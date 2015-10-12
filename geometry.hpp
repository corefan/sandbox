#ifndef geometry_H
#define geometry_H

#include "linear_algebra.hpp"
#include "math_util.hpp"
#include "geometric.hpp"
#include "GlMesh.hpp"

#include <vector>

namespace util
{
    
    struct Geometry
    {
        std::vector<math::float3> vertices;
        std::vector<math::float3> normals;
        std::vector<math::float4> colors;
        std::vector<math::float2> texCoords;
        std::vector<math::float3> tangents;
        std::vector<math::float3> bitangents;
        std::vector<math::uint3> faces;
        
        void compute_normals()
        {
            normals.resize(vertices.size());
            
            for (auto & n : normals)
                n = math::float3(0,0,0);
            
            for (const auto & tri : faces)
            {
                math::float3 n = math::cross(vertices[tri.y] - vertices[tri.x], vertices[tri.z] - vertices[tri.x]);
                normals[tri.x] += n;
                normals[tri.y] += n;
                normals[tri.z] += n;
            }
            
            for (auto & n : normals)
                n = math::normalize(n);
        }
        
        // Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh".
        // Terathon Software 3D Graphics Library, 2001.
        void compute_tangents()
        {
            //assert(normals.size() == vertices.size());
            
            tangents.resize(vertices.size());
            bitangents.resize(vertices.size());
            
            // Each face
            for (size_t i = 0; i < faces.size(); ++i)
            {
                math::uint3 face = faces[i];
                
                const math::float3 & v0 = vertices[face.x];
                const math::float3 & v1 = vertices[face.y];
                const math::float3 & v2 = vertices[face.z];
                
                const math::float2 & w0 = texCoords[face.x];
                const math::float2 & w1 = texCoords[face.y];
                const math::float2 & w2 = texCoords[face.z];
                
                // std::cout << "x: " << face.x << " y: " <<  face.y << " z: " << face.z << std::endl;
                
                float x1 = v1.x - v0.x;
                float x2 = v2.x - v0.x;
                float y1 = v1.y - v0.y;
                float y2 = v2.y - v0.y;
                float z1 = v1.z - v0.z;
                float z2 = v2.z - v0.z;
                
                float s1 = w1.x - w0.x;
                float s2 = w2.x - w0.x;
                
                float t1 = w1.y - w0.y;
                float t2 = w2.y - w0.y;
                
                float r = (s1 * t2 - s2 * t1);
                
                if (r != 0.0f)
                    r = 1.0f / r;
                
                // Tangent in the S direction
                math::float3 tangent((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
                
                // Accumulate
                tangents[face.x] += tangent;
                tangents[face.y] += tangent;
                tangents[face.z] += tangent;
            }
            
            // Tangents
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                const math::float3 normal = normals[i];
                const math::float3 tangent = tangents[i];
                
                // Gram-Schmidt orthogonalize
                tangents[i] = (tangent - normal * math::dot(normal, tangent));
                
                const float len = math::lengthL2(tangents[i]);
                
                if (len > 0.0f)
                    tangents[i] = tangents[i] / (float) sqrt(len);
            }
            
            // Bitangents 
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                bitangents[i] = math::normalize(math::cross(normals[i], tangents[i]));
            }
        }
        
        math::Box<float, 3> compute_bounds()
        {
            math::Box<float, 3> bbox;
            bbox.min() = math::float3(std::numeric_limits<float>::infinity());
            bbox.max() = -bbox.min();
            for (const auto & vertex : vertices)
            {
                bbox.min() = min(bbox.min(), vertex);
                bbox.max() = max(bbox.max(), vertex);
            }
            return bbox;
        }

    };
    
    struct Model
    {
        gfx::GlMesh mesh;
        math::Pose pose;
        math::Box<float, 3> bounds;
        
        void draw() const
        {
            mesh.draw_elements();
        }
    };
    
    inline Model make_model_from_geometry(Geometry & geometry)
    {
        Model newModel;
        
        int vertexOffset = 0;
        int normalOffset = 0;
        int colorOffset = 0;
        int texOffset = 0;
        int tanOffset = 0;
        int bitanOffset = 0;
        
        int components = 3;
        
        if (geometry.normals.size() != 0 )
        {
            normalOffset = components; components += 3;
        }
        
        if (geometry.colors.size() != 0)
        {
            colorOffset = components; components += 3;
        }
        
        if (geometry.texCoords.size() != 0)
        {
            texOffset = components; components += 2;
        }
        
        if (geometry.tangents.size() != 0)
        {
            tanOffset = components; components += 3;
        }
        
        if (geometry.bitangents.size() != 0)
        {
            bitanOffset = components; components += 3;
        }
        
        std::vector<float> buffer;
        buffer.reserve(geometry.vertices.size() * components);
        
        for (size_t i = 0; i < geometry.vertices.size(); ++i)
        {
            buffer.push_back(geometry.vertices[i].x);
            buffer.push_back(geometry.vertices[i].y);
            buffer.push_back(geometry.vertices[i].z);
            
            if (normalOffset)
            {
                buffer.push_back(geometry.normals[i].x);
                buffer.push_back(geometry.normals[i].y);
                buffer.push_back(geometry.normals[i].z);
            }
            
            if (colorOffset)
            {
                buffer.push_back(geometry.colors[i].x);
                buffer.push_back(geometry.colors[i].y);
                buffer.push_back(geometry.colors[i].z);
            }
            
            if (texOffset)
            {
                buffer.push_back(geometry.texCoords[i].x);
                buffer.push_back(geometry.texCoords[i].y);
            }
            
            if (tanOffset)
            {
                buffer.push_back(geometry.tangents[i].x);
                buffer.push_back(geometry.tangents[i].y);
                buffer.push_back(geometry.tangents[i].z);
            }
            
            if (bitanOffset)
            {
                buffer.push_back(geometry.bitangents[i].x);
                buffer.push_back(geometry.bitangents[i].y);
                buffer.push_back(geometry.bitangents[i].z);
            }
        }
        
        gfx::GlMesh & m = newModel.mesh;
        
        m.set_vertex_data(buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);
        m.set_attribute(0, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + vertexOffset);
        if (normalOffset) m.set_attribute(1, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + normalOffset);
        if (colorOffset) m.set_attribute(2, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + colorOffset);
        if (texOffset) m.set_attribute(3, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + texOffset);
        if (tanOffset) m.set_attribute(4, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + tanOffset);
        if (bitanOffset) m.set_attribute(5, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), ((float*) 0) + bitanOffset);
        
        if (geometry.faces.size() > 0)
            m.set_elements(geometry.faces, GL_STATIC_DRAW);
        
        return newModel;
    }
    
} // end namespace util

#endif // geometry_h
