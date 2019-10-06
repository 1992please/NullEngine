#include "NullEngine.h"
#include "Math/RotationTranslationMatrix.h"
#include "GL/gl3w.h"

NullEngine::NullEngine()
{
}

void NullEngine::StartUp()
{
	mShader.CompileShader("Engine/Shaders/TestShader.glsl");

	GLuint blockIndex = glGetUniformBlockIndex(mShader.GetID(),
		"BlobSettings");

	GLint blockSize;
	glGetActiveUniformBlockiv(mShader.GetID(), blockIndex,
		GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	GLubyte * blockBuffer;
	blockBuffer = (GLubyte *)malloc(blockSize);

	const GLchar *names[] = { "InnerColor", "OuterColor",
	   "RadiusInner", "RadiusOuter" };
	GLuint indices[4];
	glGetUniformIndices(mShader.GetID(), 4, names, indices);

	GLint offset[4];
	glGetActiveUniformsiv(mShader.GetID(), 4, indices,
		GL_UNIFORM_OFFSET, offset);

	// Store data within the buffer at the appropriate offsets 
	GLfloat outerColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 0.0f, 0.75f, 1.0f };
	GLfloat innerRadius = 0.25f, outerRadius = 0.45f;

	memcpy(blockBuffer + offset[0], innerColor,
		4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[1], outerColor,
		4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &innerRadius,
		sizeof(GLfloat));
	memcpy(blockBuffer + offset[3], &outerRadius,
		sizeof(GLfloat));

	GLuint uboHandle;
	glGenBuffers(1, &uboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer,
		GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);

    float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.8f,  0.8f, 0.0f,
         -0.8f, -0.8f, 0.0f,
          0.8f, 0.8f, 0.0f,
          -0.8f, 0.8f, 0.0f
     };
    float tcData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

	// Create and populate the buffer objects
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint tcBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tcBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), tcData, GL_STATIC_DRAW);

	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, tcBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void NullEngine::Update(float DeltaTime)
{
	angle += DeltaTime * 10;
	if (angle >= 360.0f) angle -= 360.0f;
}

void NullEngine::Render()
{
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };

	glClearBufferfv(GL_COLOR, 0, green);
	mShader.Use();

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	mShader.UnUse();
}

void NullEngine::Shutdown()
{
	mShader.DeleteShaderProgram();
}

void NullEngine::Init()
{
	EngineBase::Init();
	//EngineConfigs.flags.fullscreen = 1;
}

NullEngine::~NullEngine()
{
}
