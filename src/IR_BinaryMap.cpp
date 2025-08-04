#include <IR_BinaryMap.hpp>

#include <fstream>

namespace IR {

    constexpr UInt32 MAGIC = 0x6D627269; // irbm

    enum BMLumpInfoType {
        LUMPTYPE_ENTITIES,
        LUMPTYPE_BRUSHES,
        LUMPTYPE_FACES,
        LUMPTYPE_VERTICES,
        LUMPTYPE_MATERIALTABLE,
        LUMPTYPE__COUNT
    };

    struct BMLumpInfo {
        UInt32 offset;
        UInt32 length;
    };

    struct BMHeader {
        UInt32 magic;
        UInt32 version;

        BMLumpInfo lumps[LUMPTYPE__COUNT];
    };

    struct BMEntity {
        UInt32 kvNum; // Number of KV pairs, used to know when to stop reading
        std::vector<char> kv; // Just a big focking null terminated strings of kv pairs
        UInt32 brushNum; // number of brushes to read after first
        UInt32 brushBegin; // first brush
    };

    struct BMBrush {
        UInt32 faceNum;
        UInt32 faceBegin;
        glm::vec3 origin;
        std::vector<glm::vec3> convexPoints;
    };

    struct BMFace {
        glm::vec4 plane;
        UInt32 flags;
        UInt32 matNameOffset;
        UInt32 vertNum;
        UInt32 vertBegin;
        std::vector<UInt32> indices;
    };

    struct BMVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    static void ReadEntities(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<BMEntity>& ents)
    {
        stream.seekg(pos);

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            BMEntity tmpEnt;
            stream.read((char*)&tmpEnt.kvNum, sizeof(tmpEnt.kvNum));

            tmpEnt.kv.reserve(tmpEnt.kvNum * 2 + 16); // Minor optimization and assumption

            char ch;
            UInt32 kvCounter = 0;
            while (kvCounter < tmpEnt.kvNum * 2 && stream.get(ch)) {
                if (ch == '\0') {
                    kvCounter++;
                }

                // Emplacing these 1 by 1 maybe slower than getting length then reserve and emplace.
                tmpEnt.kv.emplace_back(ch);
            }

            stream.read((char*)&tmpEnt.brushNum, sizeof(tmpEnt.brushNum));
            stream.read((char*)&tmpEnt.brushBegin, sizeof(tmpEnt.brushBegin));

            ents.emplace_back(tmpEnt);
        }
    }

    static void ReadBrushes(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<BMBrush>& brushes)
    {
        stream.seekg(pos);

        BMBrush brush;
        glm::vec3 tmpVec;
        UInt32 convexVecCount;

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            brush.convexPoints.clear();

            stream.read((char*)&brush.faceNum, sizeof(brush.faceNum));
            stream.read((char*)&brush.faceBegin, sizeof(brush.faceBegin));

            stream.read((char*)&brush.origin.x, sizeof(brush.origin.x));
            stream.read((char*)&brush.origin.y, sizeof(brush.origin.y));
            stream.read((char*)&brush.origin.z, sizeof(brush.origin.z));

            stream.read((char*)&convexVecCount, sizeof(convexVecCount));
            brush.convexPoints.reserve(convexVecCount);
            for (UInt32 i = 0; i < convexVecCount; i++) {
                stream.read((char*)&tmpVec.x, sizeof(tmpVec.x));
                stream.read((char*)&tmpVec.y, sizeof(tmpVec.y));
                stream.read((char*)&tmpVec.z, sizeof(tmpVec.z));

                brush.convexPoints.emplace_back(tmpVec);
            }

            brushes.emplace_back(brush);
        }
    }

    static void ReadFaces(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<BMFace>& faces)
    {
        stream.seekg(pos);

        BMFace face;
        UInt32 indexCount;

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            face.indices.clear();

            stream.read((char*)&face.plane.x, sizeof(face.plane.x));
            stream.read((char*)&face.plane.y, sizeof(face.plane.y));
            stream.read((char*)&face.plane.z, sizeof(face.plane.z));
            stream.read((char*)&face.plane.w, sizeof(face.plane.w));

            stream.read((char*)&face.flags, sizeof(face.flags));
            stream.read((char*)&face.matNameOffset, sizeof(face.matNameOffset));

            stream.read((char*)&face.vertNum, sizeof(face.vertNum));
            stream.read((char*)&face.vertBegin, sizeof(face.vertBegin));
            stream.read((char*)&indexCount, sizeof(indexCount));

            face.indices.reserve(indexCount);
            for (UInt32 i = 0; i < indexCount; i++) {
                UInt32 index = 0;
                stream.read((char*)&index, sizeof(index));
                face.indices.emplace_back(index);
            }

            faces.emplace_back(face);
        }
    }

    static void ReadVertices(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<BMVertex>& vertices)
    {
        stream.seekg(pos);

        BMVertex vert;

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            stream.read((char*)&vert.position.x, sizeof(vert.position.x));
            stream.read((char*)&vert.position.y, sizeof(vert.position.y));
            stream.read((char*)&vert.position.z, sizeof(vert.position.z));

            stream.read((char*)&vert.normal.x, sizeof(vert.normal.x));
            stream.read((char*)&vert.normal.y, sizeof(vert.normal.y));
            stream.read((char*)&vert.normal.z, sizeof(vert.normal.z));

            stream.read((char*)&vert.texcoord.x, sizeof(vert.texcoord.x));
            stream.read((char*)&vert.texcoord.y, sizeof(vert.texcoord.y));

            vertices.emplace_back(vert);
        }
    }

    static void ReadMaterialTable(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<char>& materialtable)
    {
        materialtable.resize(len);
        stream.seekg(pos);
        stream.read(materialtable.data(), len);
    }

    bool BinaryMap::Load(const char* path)
    {
        std::ifstream stream(("assets/maps/" + std::string(path)).c_str(), std::ios::binary);
        if (!stream.is_open()) {
            IR_MSG(ERROR, "Couldn't load Binary Map: couldn't open the file %s", path);
            return false;
        }

        BMHeader hdr;
        stream.read((char*)&hdr, sizeof(hdr));

        if (hdr.magic != MAGIC) {
            IR_MSG(ERROR, "Couldn't load Binary Map: this could have happened because, the file may not be a \"irbm\" file. Or you have ARM CPU, we gotta fix that.");
            return false;
        }

        std::vector<BMEntity> ents;
        ReadEntities(stream, hdr.lumps[LUMPTYPE_ENTITIES].offset, hdr.lumps[LUMPTYPE_ENTITIES].length, ents);

        std::vector<BMBrush> brushes;
        ReadBrushes(stream, hdr.lumps[LUMPTYPE_BRUSHES].offset, hdr.lumps[LUMPTYPE_BRUSHES].length, brushes);

        std::vector<BMFace> faces;
        ReadFaces(stream, hdr.lumps[LUMPTYPE_FACES].offset, hdr.lumps[LUMPTYPE_FACES].length, faces);

        std::vector<BMVertex> vertices;
        ReadVertices(stream, hdr.lumps[LUMPTYPE_VERTICES].offset, hdr.lumps[LUMPTYPE_VERTICES].length, vertices);

        std::vector<char> materialtable;
        ReadMaterialTable(stream, hdr.lumps[LUMPTYPE_MATERIALTABLE].offset, hdr.lumps[LUMPTYPE_MATERIALTABLE].length, materialtable);

        for (const auto& ent : ents) {
            EntityData entdata;
            entdata.brushes.reserve(ent.brushNum);
            for (UInt32 brushIndex = ent.brushBegin; brushIndex < ent.brushBegin + ent.brushNum; brushIndex++) {
                const BMBrush& brush = brushes[brushIndex];
                BrushData brushdata;

                brushdata.faces.reserve(brush.faceNum);
                for (UInt32 faceIndex = brush.faceBegin; faceIndex < brush.faceBegin + brush.faceNum; faceIndex++) {
                    const BMFace& face = faces[faceIndex];
                    FaceData facedata;

                    facedata.plane = face.plane;
                    facedata.flags = face.flags;

                    char* matNamePtr = materialtable.data() + face.matNameOffset;
                    facedata.materialName = std::string(matNamePtr + 1, *(UInt8*)matNamePtr) + ".shader";

                    facedata.vertices.reserve(face.vertNum);

                    for (UInt32 vertIndex = face.vertBegin; vertIndex < face.vertBegin + face.vertNum; vertIndex++) {
                        const BMVertex& vert = vertices[vertIndex];
                        const Renderer::VertexStandard standardVert = { vert.position, vert.normal, vert.texcoord };

                        facedata.vertices.emplace_back(standardVert);
                    }

                    facedata.indices = face.indices;

                    brushdata.faces.emplace_back(facedata);
                }

                brushdata.convexPoints.reserve(brush.convexPoints.size());
                brushdata.convexPoints.insert(brushdata.convexPoints.end(), brush.convexPoints.begin(), brush.convexPoints.end());

                brushdata.origin = brush.origin;

                entdata.brushes.emplace_back(brushdata);
            }

            char* kvPtr = (char*)ent.kv.data();
            for (UInt32 i = 0; i < ent.kvNum; i++) {
                const std::string key = kvPtr;
                kvPtr += key.size() + 1;
                const std::string value = kvPtr;
                kvPtr += value.size() + 1;

                entdata.keyvalues[key] = value;
            }

            m_EntityDatas.emplace_back(entdata);
        }

        return true;
    }

}