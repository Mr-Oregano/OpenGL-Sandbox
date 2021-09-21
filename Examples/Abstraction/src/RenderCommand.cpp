#include "RenderCommand.h"

void RenderCommand::DrawIndexed(const GLCore::Ref<VertexArray> &vao)
{
	glDrawElements(vao->GetPrimitiveType(), vao->GetElementBuffer().GetElementCount(), GL_UNSIGNED_INT, nullptr);
}
void RenderCommand::DrawIndexed(const GLCore::Ref<VertexArray> &vao, unsigned int count)
{
	glDrawElements(vao->GetPrimitiveType(), count, GL_UNSIGNED_INT, nullptr);
}

void RenderCommand::Draw(const GLCore::Ref<VertexArray> &vao)
{
	glDrawArrays(vao->GetPrimitiveType(), 0, vao->GetVertexCount());
}
void RenderCommand::Draw(const GLCore::Ref<VertexArray> &vao, unsigned int count)
{
	glDrawArrays(vao->GetPrimitiveType(), 0, count);
}

void RenderCommand::DrawIndexed(const VertexArray &vao)
{
	glDrawElements(vao.GetPrimitiveType(), vao.GetElementBuffer().GetElementCount(), GL_UNSIGNED_INT, nullptr);
}
void RenderCommand::DrawIndexed(const VertexArray &vao, unsigned int count)
{
	glDrawElements(vao.GetPrimitiveType(), count, GL_UNSIGNED_INT, nullptr);
}

void RenderCommand::Draw(const VertexArray &vao)
{
	glDrawArrays(vao.GetPrimitiveType(), 0, vao.GetVertexCount());
}
void RenderCommand::Draw(const VertexArray &vao, unsigned int count)
{
	glDrawArrays(vao.GetPrimitiveType(), 0, count);
}
