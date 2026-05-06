#include <nel-tools/usd/mesh-converter/ConverterCMesh.h>

#include <fmt/color.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;
using namespace pxr;

uint32 getIndexAt(const CIndexBufferRead &buffer, const int index)
{
	switch (buffer.getFormat())
	{
		case CIndexBuffer::Indices32:
			return *(static_cast<const uint32 *>(buffer.getPtr()) + index);
		case CIndexBuffer::Indices16:
		default:
			return *(static_cast<const uint16 *>(buffer.getPtr()) + index);
		}
}


void ConverterCMesh::convert(UsdStageRefPtr &output)
{
	UsdGeomSetStageUpAxis(output, UsdGeomTokens->z);
    auto modelRoot = UsdGeomXform::Define(output,  SdfPath("/shape"));
	auto outMesh = UsdGeomMesh::Define(output, SdfPath("/shape/mesh"));
 	auto attributePoints = outMesh.CreatePointsAttr();
	auto attributeIndices = outMesh.CreateFaceVertexIndicesAttr();
	auto attributeFaceCount = outMesh.CreateFaceVertexCountsAttr();

	CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
	CVertexBufferRead vba;
	vertexBuffer.lock(vba);
	VtArray<GfVec3f> vertexArray;
	for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
	{
		auto vertex = *vba.getVertexCoordPointer(i);
		vertexArray.emplace_back(vertex.x, vertex.y, vertex.z);
	}
	attributePoints.Set(vertexArray);

    constexpr uint lodId = 0;
	VtArray<int> indices;
	for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
	{
		auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
		auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
		nlinfo("RenderPasss %i Elements %i Material %i", renderPass, indexBuffer.getNumIndexes(), materialIndex);
		auto material = mesh->getMaterial(materialIndex);
		vector<string> textures;

		CIndexBufferRead iba;
		indexBuffer.lock(iba);

		for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
		{
			if (uint32 idx = getIndexAt(iba, i); idx != -1)
			{
				indices.emplace_back(idx);
			}
		}
		nldebug("index min %i max %i", *min_element(indices.begin(), indices.end()), *max_element(indices.begin(), indices.end()));
	}
	attributeIndices.Set(indices);
	VtArray<int> faceCount;
	for (auto i = 0; i < indices.size(); i += 3)
	{
		faceCount.emplace_back(3);
	}
	attributeFaceCount.Set(faceCount);
}
