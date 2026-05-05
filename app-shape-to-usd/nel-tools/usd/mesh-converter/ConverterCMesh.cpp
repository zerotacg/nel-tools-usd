#include <nel-tools/usd/mesh-converter/ConverterCMesh.h>

#include <fmt/color.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;
using namespace pxr;

void ConverterCMesh::process(UsdStageRefPtr &output)
{
	CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
	CVertexBufferRead vba;
	vertexBuffer.lock(vba);
	const uint lodId = 0;
	const auto lodCount = mesh->getNbMatrixBlock();
	fmt::print( "LodCount {}\n", lodCount);

	for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
	{
	}

	for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
	{
		auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
		auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
		nlinfo("RenderPasss %i Elements %i Material %i", renderPass, indexBuffer.getNumIndexes(), materialIndex);
		auto material = mesh->getMaterial(materialIndex);
		vector<string> textures;
	}}
