#include <IR_AssetBMap.hpp>

#include <fstream>

namespace IR::Asset::BMap {

    constexpr UInt32 MAGIC = 0x6D627269; // irbm

    enum LumpInfoType {
        LUMPTYPE_ENTITIES,
        LUMPTYPE_BRUSHES,
        LUMPTYPE_FACES,
        LUMPTYPE_VERTICES,
        LUMPTYPE__COUNT
    };

    struct LumpInfo {
        UInt32 offset;
        UInt32 length;
    };

    struct Header {
        UInt32 magic;
        UInt32 version;

        LumpInfo lumps[LUMPTYPE__COUNT];
    };

    struct Entity {
        UInt32 kvNum; // Number of KV pairs, used to know when to stop reading
        std::vector<char> kv; // Just a big focking null terminated strings of kv pairs
        UInt32 brushNum; // number of brushes to read after first
        UInt32 brushBegin; // first brush
    };

    struct Brush {
        UInt32 faceNum;
        UInt32 faceBegin;
        std::vector<glm::vec3> convexPoints;
    };

    struct Face {
        glm::vec4 plane;
        UInt32 flags;
        UInt32 vertNum;
        UInt32 vertBegin;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    static void ReadEntities(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<Entity>& ents)
    {
        stream.seekg(pos);

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            Entity tmpEnt;
            stream.read((char*)&tmpEnt.kvNum, sizeof(tmpEnt.kvNum));

            tmpEnt.kv.reserve(tmpEnt.kvNum * 2 + 16);

            char ch;
            UInt32 kvCounter = 0;
            while (stream.get(ch) && kvCounter < tmpEnt.kvNum * 2) {
                if (ch == '\0') {
                    kvCounter++;
                }

                tmpEnt.kv.push_back(ch);
            }

            stream.read((char*)&tmpEnt.brushNum, sizeof(tmpEnt.brushNum));
            stream.read((char*)&tmpEnt.brushBegin, sizeof(tmpEnt.brushBegin));

            ents.push_back(tmpEnt);
        }
    }

    static void ReadBrushes(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<Brush>& brushes)
    {
        stream.seekg(pos);

        Brush brush;
        glm::vec3 tmpVec;
        UInt32 convexVecCount;

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            brush.convexPoints.clear();

            stream.read((char*)&brush.faceNum, sizeof(brush.faceNum));
            stream.read((char*)&brush.faceBegin, sizeof(brush.faceBegin));

            stream.read((char*)&convexVecCount, sizeof(convexVecCount));

            for (UInt32 i = 0; i < convexVecCount; i++) {
                stream.read((char*)&tmpVec.x, sizeof(tmpVec.x));
                stream.read((char*)&tmpVec.y, sizeof(tmpVec.y));
                stream.read((char*)&tmpVec.z, sizeof(tmpVec.z));

                brush.convexPoints.push_back(tmpVec);
            }

            brushes.push_back(brush);
        }
    }

    static void ReadFaces(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<Face>& faces)
    {
        stream.seekg(pos);

        Face face;

        UInt32 end = pos + len;
        while (stream.tellg() < end) {
            stream.read((char*)&face.plane.x, sizeof(face.plane.x));
            stream.read((char*)&face.plane.y, sizeof(face.plane.y));
            stream.read((char*)&face.plane.z, sizeof(face.plane.z));
            stream.read((char*)&face.plane.w, sizeof(face.plane.w));

            stream.read((char*)&face.flags, sizeof(face.flags));

            stream.read((char*)&face.vertNum, sizeof(face.vertNum));
            stream.read((char*)&face.vertBegin, sizeof(face.vertBegin));

            faces.push_back(face);
        }
    }

    static void ReadVertices(std::ifstream& stream, UInt32 pos, UInt32 len, std::vector<Vertex>& vertices)
    {
        stream.seekg(pos);

        Vertex vert;

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

            vertices.push_back(vert);
        }
    }

    IR::BMap Load(const char* path)
    {
        IR::BMap bmap;

        std::ifstream stream(("assets/maps/" + std::string(path)).c_str(), std::ios::binary);
        if (!stream.is_open()) {
            IR_MSG(FATAL, "Couldn't load Binary Map: couldn't open the file %s", path);
        }

        Header hdr;
        stream.read((char*)&hdr, sizeof(hdr));

        if (hdr.magic != MAGIC) {
            IR_MSG(FATAL, "Couldn't load Binary Map: this could have happened because, the file may not be a \"irbm\" file. Or you have ARM CPU, we gotta fix that.");
        }

        IR_MSG(INFO, "Entities: %d, %d", hdr.lumps[LUMPTYPE_ENTITIES].offset, hdr.lumps[LUMPTYPE_ENTITIES].length);
        IR_MSG(INFO, "Brushes: %d, %d", hdr.lumps[LUMPTYPE_BRUSHES].offset, hdr.lumps[LUMPTYPE_BRUSHES].length);
        IR_MSG(INFO, "Faces: %d, %d", hdr.lumps[LUMPTYPE_FACES].offset, hdr.lumps[LUMPTYPE_FACES].length);
        IR_MSG(INFO, "Vertices: %d, %d", hdr.lumps[LUMPTYPE_VERTICES].offset, hdr.lumps[LUMPTYPE_VERTICES].length);

        std::vector<Entity> ents;
        ReadEntities(stream, hdr.lumps[LUMPTYPE_ENTITIES].offset, hdr.lumps[LUMPTYPE_ENTITIES].length, ents);

        std::vector<Brush> brushes;
        ReadBrushes(stream, hdr.lumps[LUMPTYPE_BRUSHES].offset, hdr.lumps[LUMPTYPE_BRUSHES].length, brushes);

        std::vector<Face> faces;
        ReadFaces(stream, hdr.lumps[LUMPTYPE_FACES].offset, hdr.lumps[LUMPTYPE_FACES].length, faces);

        std::vector<Vertex> vertices;
        ReadVertices(stream, hdr.lumps[LUMPTYPE_VERTICES].offset, hdr.lumps[LUMPTYPE_VERTICES].length, vertices);

        return bmap;
    }

}