#include "rendering/shader.hpp"

#include "engine/core/util.hpp"

#define SHADER_INFO_LOG_SIZE	1024

static bool addShader(GLuint program, const String& text,
		GLenum type, ArrayList<uint32>& shaders);
static bool checkShaderError(uint32 shader, GLenum flag,
		bool isProgram, const String& errorMessage);

bool Shader::load(const String& fileName, const char** feedbackVaryings,
		uintptr numFeedbackVaryings, uint32 varyingCaptureMode) {
	StringStream ss;
	
	if (!Util::loadFileWithLinking(ss, fileName, "#include")) {
		DEBUG_LOG(LOG_ERROR, "Shader",
				"Failed to load shader file with includes: %s",
				fileName.c_str());

		return false;
	}

	const String text = ss.str();

	if (programID != 0) {
		cleanUp();
	}

	programID = glCreateProgram();

	const String version = "#version " + context->getShaderVersion()
		+ "\n#define GLSL_VERSION " + context->getShaderVersion();

	if (text.find("CS_BUILD") != String::npos) {
		const String computeShaderText = version
			+ "\n#define CS_BUILD\n" + text;

		if (!addShader(programID, computeShaderText, GL_COMPUTE_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load compute shader: %s",
					fileName.c_str());

			return false;
		}
	}
	else {
		const String vertexShaderText = version
			+ "\n#define VS_BUILD\n" + text;
		const String fragmentShaderText = version
			+ "\n#define FS_BUILD\n" + text;

		if (!addShader(programID, vertexShaderText, GL_VERTEX_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load vertex shader: %s",
					fileName.c_str());

			return false;
		}

		if (!addShader(programID, fragmentShaderText, GL_FRAGMENT_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load fragment shader: %s",
					fileName.c_str());

			return false;
		}

		if (text.find("GS_BUILD") != String::npos) {
			String geomShaderText = version
				+ "\n#define GS_BUILD\n" + text;

			if (!addShader(programID, geomShaderText, GL_GEOMETRY_SHADER, shaders)) {
				DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load geometry shader: %s",
						fileName.c_str());

				return false;
			}
		}
	}

	if (feedbackVaryings != nullptr && numFeedbackVaryings > 0) {
		glTransformFeedbackVaryings(programID, numFeedbackVaryings,
				feedbackVaryings, varyingCaptureMode);
	}

	glLinkProgram(programID);

	if (checkShaderError(programID, GL_LINK_STATUS, true,
				"Error linking shader program")) {
		DEBUG_LOG(LOG_ERROR, "Shader", "Error linking shader program: %s",
				fileName.c_str());

		return false;
	}

	glValidateProgram(programID);

	if (checkShaderError(programID, GL_VALIDATE_STATUS, true,
				"Invalid shader program")) {
		DEBUG_LOG(LOG_ERROR, "Shader", "Invalid shader program: %s",
				fileName.c_str());

		return false;
	}

	// TODO: add attributes
	addUniforms();

	return true;
}

void Shader::setUniformBuffer(const String& name,
		Memory::SharedPointer<UniformBuffer> buffer) {
	glUniformBlockBinding(programID, uniformBlockMap[name], buffer->getBlockBinding());
	uniformBuffers[name] = buffer;
}

void Shader::setShaderStorageBuffer(const String& name,
		ShaderStorageBuffer& buffer) {
	glUniformBlockBinding(programID, uniformBlockMap[name],
			buffer.getBlockBinding());
}

void Shader::setSampler(const String& name, Texture& texture,
		Sampler& sampler, uint32 textureUnit) {
	context->setShader(programID);

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture.getID());
	glBindSampler(textureUnit, sampler.getID());
	glUniform1i(samplerMap[name], textureUnit);
}

void Shader::setSampler(const String& name, CubeMap& cubeMap,
		Sampler& sampler, uint32 textureUnit) {
	context->setShader(programID);

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getID());
	glBindSampler(textureUnit, sampler.getID());
	glUniform1i(samplerMap[name], textureUnit);
}

void Shader::bindComputeTexture(Texture& texture, uint32 unit,
		uint32 access, uint32 internalFormat) {
	context->setShader(programID);
	glBindImageTexture(unit, texture.getID(), 0, false, 0,
			access, internalFormat);
}

void Shader::setInt(const String& name, int32 value) {
	context->setShader(programID);
	glUniform1i(uniformMap[name], value);
}

void Shader::setFloat(const String& name, float value) {
	context->setShader(programID);
	glUniform1f(uniformMap[name], value);
}

void Shader::setVector2f(const String& name, const Vector2f& value) {
	context->setShader(programID);
	glUniform2fv(uniformMap[name], 1, (const float*)&value);
}

void Shader::setVector3f(const String& name, const Vector3f& value) {
	context->setShader(programID);
	glUniform3fv(uniformMap[name], 1, (const float*)&value);
}

void Shader::setMatrix4f(const String& name, const Matrix4f& value) {
	context->setShader(programID);
	glUniformMatrix4fv(uniformMap[name], 1, false, (const float*)&value);
}

Memory::SharedPointer<UniformBuffer> Shader::getUniformBuffer(const String& name) {
	return uniformBuffers[name];
}

Shader::~Shader() {
	cleanUp();
}

void Shader::cleanUp() {
	for (auto& shader : shaders) {
		glDetachShader(programID, shader);
		glDeleteShader(shader);
	}

	glDeleteProgram(programID);
	context->setShader(0);

	programID = 0;

	shaders.clear();
	uniformBlockMap.clear();
	samplerMap.clear();
	uniformMap.clear();
}

void Shader::addUniforms() {
	GLint numBlocks;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

	ArrayList<GLchar> nameBuffer;
	ArrayList<GLchar> uniformName(256);

	for (int32 block = 0; block < numBlocks; ++block) {
		GLint nameLen;

		glGetActiveUniformBlockiv(programID, block,
				GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
		nameBuffer.reserve(nameLen);

		glGetActiveUniformBlockName(programID, block,
				nameLen, nullptr, nameBuffer.data());

		String uniformBlockName(nameBuffer.data(), nameLen - 1);

		uniformBlockMap[uniformBlockName] = glGetUniformBlockIndex(programID, nameBuffer.data());

		if (auto ptr = context->getUniformBuffer(uniformBlockName); ptr.expired()) {
			GLint numUniforms;
			GLint bufferSize;

			glGetActiveUniformBlockiv(programID, block,
					GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);
			glGetActiveUniformBlockiv(programID, block,
					GL_UNIFORM_BLOCK_DATA_SIZE, &bufferSize);

			ArrayList<GLint> uniformIndices(numUniforms);

			glGetActiveUniformBlockiv(programID, block,
					GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

			auto ubo = context->addUniformBuffer(uniformBlockName, bufferSize, GL_DYNAMIC_DRAW);

			for (auto& uniform : uniformIndices) {
				const GLuint index = static_cast<GLuint>(uniform);

				GLint offset = 0;
				GLint arraySize = 0;
				GLenum type = 0;
				GLsizei actualLength = 0;

				glGetActiveUniform(programID, uniform, uniformName.size(),
						&actualLength, &arraySize, &type, uniformName.data());

				glGetActiveUniformsiv(programID, 1, &index,
						GL_UNIFORM_OFFSET, &offset);

				String name(uniformName.data(), actualLength);

				ubo->addVariableOffset(name, offset);
			}

			uniformBuffers[uniformBlockName] = ubo;
			setUniformBuffer(uniformBlockName, ubo);
		}
		else {
			auto ubo = ptr.lock();

			uniformBuffers[uniformBlockName] = ubo;
			setUniformBuffer(uniformBlockName, ubo);
		}
	}

	GLint numUniforms = 0;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);

	for (int32 uniform = 0; uniform < numUniforms; ++uniform) {
		const GLuint iUniform = static_cast<GLuint>(uniform);

		GLint uniformBlockIndex;

		glGetActiveUniformsiv(programID, 1, &iUniform,
				GL_UNIFORM_BLOCK_INDEX, &uniformBlockIndex);

		if (uniformBlockIndex < 0) {
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			glGetActiveUniform(programID, uniform, uniformName.size(),
					&actualLength, &arraySize, &type, uniformName.data());

			String name(uniformName.data(), actualLength);

			if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) {
				samplerMap[name] = glGetUniformLocation(programID, uniformName.data());
			}
			else {
				uniformMap[name] = glGetUniformLocation(programID, uniformName.data());
			}
		}
	}
}

static bool addShader(GLuint program, const String& text,
		GLenum type, ArrayList<uint32>& shaders) {
	uint32 shader = glCreateShader(type);

	if (shader == 0) {
		DEBUG_LOG("Shader", LOG_ERROR, "Error creating shader type %d", type);
		return false;
	}

	const GLchar* p[1] = { text.c_str() };
	GLint lengths[1] = { (GLint)text.length() };

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status) {
		GLchar infoLog[SHADER_INFO_LOG_SIZE];
		glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, infoLog);

		DEBUG_LOG("Shader", LOG_ERROR, "Error compiling shader type %d: '%s'\n",
				type, infoLog);

		return false;
	}

	glAttachShader(program, shader);
	shaders.push_back(shader);

	return true;
}

static bool checkShaderError(uint32 shader, GLenum flag,
		bool isProgram, const String& errorMessage) {
	GLint status = 0;
	GLchar error[SHADER_INFO_LOG_SIZE];

	if (isProgram) {
		glGetProgramiv(shader, flag, &status);
	}
	else {
		glGetShaderiv(shader, flag, &status);
	}

	if (!status) {
		if (isProgram) {
			glGetProgramInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, error);
		}
		else {
			glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, error);
		}

		DEBUG_LOG("Shader", LOG_ERROR, "%s: '%s'\n",
				errorMessage.c_str(), error);

		return true;
	}

	return false;
}
