/*

    3DS model loader
    � Keith O'Conor 2005
    keith.oconor @ {cs.tcd.ie, gmail.com}
*/
/*
Copyright (C) 2012 RoomEdit Development Team

This file is part of RoomEdit.

RoomEdit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RoomEdit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RoomEdit. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MODEL3DS_H
#define MODEL3DS_H

#ifdef Q_WS_WIN
#  define WIN32_LEAN_AND_MEAN
#  include "windows.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <assert.h>
#include <math.h>

#ifdef Q_WS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "RTexture.h"
#include "global.h"

#define CHUNK_MAIN                                      0x4D4D
#define CHUNK_3D_EDITOR                         0x3D3D
#define CHUNK_OBJECT_BLOCK                      0x4000
#define CHUNK_MESH                                      0x4100
#define CHUNK_VERTICES                          0x4110
#define CHUNK_FACES                                     0x4120
#define CHUNK_FACE_MATERIAL                     0x4130
#define CHUNK_SMOOTHING_GROUP           0x4150
#define CHUNK_TEXCOORDS                         0x4140
#define CHUNK_MATERIAL_BLOCK            0xAFFF
#define CHUNK_MATERIAL_NAME                     0xA000
#define CHUNK_AMBIENT_COLOR                     0xA010
#define CHUNK_DIFFUSE_COLOR                     0xA020
#define CHUNK_SPECULAR_COLOR            0xA030
#define CHUNK_SPECULAR_EXPONENT         0xA040
#define CHUNK_SHININESS                         0xA041
#define CHUNK_TRANSPARENCY                      0xA050
#define CHUNK_MATERIAL_TWO_SIDED        0xA081
#define CHUNK_TEXTURE_MAP                       0xA200
#define CHUNK_BUMP_MAP                          0xA230
#define CHUNK_REFLECTION_MAP            0xA220
#define CHUNK_MAP_FILENAME                      0xA300
#define CHUNK_RGB_FLOAT                         0x0010
#define CHUNK_RGB_BYTE                          0x0011
#define CHUNK_RGB_BYTE_GAMMA            0x0012
#define CHUNK_RGB_FLOAT_GAMMA           0x0013
#define CHUNK_PERCENT_INT                       0x0030
#define CHUNK_PERCENT_FLOAT                     0x0031

#define DRAW_IMMEDIATE_MODE                     0x01
#define DRAW_VERTEX_ARRAY                       0x02
#define DRAW_VBO                                        0x03

#define DEBUG_OUTPUT                            false

namespace reditor
{

class RModel3DS;
class mesh3DS;
class material3DS;
class RResourceDB;

typedef unsigned short ushort;

///////////////////////////////////////////////////////
// 3DS Material
//  Describes a material to be used by a mesh3DS
/////////////////////////////////////////////////////

class material3DS
{
private:
    RResourceDB * m_rsc;
    std::string m_name;
    float m_diffuseColor[3];
    float m_ambientColor[3];
    float m_specularColor[3];
    float m_specularExponent;
    float m_shininess;
    float m_opacity;
    GLuint m_textureMapId;
    GLuint m_bumpMapId;
    bool m_hasTextureMap;
    bool m_hasBumpMap;
    bool m_twoSided;
    int m_compiledList;

public:
    explicit material3DS(RResourceDB * rsc) : m_rsc(rsc), m_hasTextureMap ( false ),m_hasBumpMap ( false ),m_twoSided ( false ),m_compiledList ( 0 ) {}
    //material3DS() : m_rsc(&rscDB), m_hasTextureMap ( false ),m_hasBumpMap ( false ),m_twoSided ( false ),m_compiledList ( 0 ) {}
    void loadTexture ( std::string filename, int mapType );

    std::string getName() const
    {
        return m_name;
    }
    const float* getDiffuseColor() const
    {
        return m_diffuseColor;
    }
    const float* getAmbientColor() const
    {
        return m_ambientColor;
    }
    const float* getSpecularColor() const
    {
        return m_specularColor;
    }
    int getList() const
    {
        return m_compiledList;
    }
    float getSpecularExponent() const
    {
        return m_specularExponent;
    }
    float getShininess() const
    {
        return m_shininess;
    }
    float getOpacity() const
    {
        return m_opacity;
    }
    GLuint getTextureMapId() const
    {
        return m_textureMapId;
    }
    GLuint getBumpMapId() const
    {
        return m_bumpMapId;
    }
    bool hasTextureMap() const
    {
        return m_hasTextureMap;
    }
    bool hasBumpMap() const
    {
        return m_hasBumpMap;
    }
    bool isTwoSided() const
    {
        return m_twoSided;
    }

    void setList ( const int listID )
    {
        m_compiledList = listID;
    }
    void setName ( const std::string newName )
    {
        m_name = newName;
    }
    void setDiffuseColor ( const float* newColor )
    {
        memcpy ( m_diffuseColor,newColor,sizeof ( float ) *3 );
    }
    void setAmbientColor ( const float* newColor )
    {
        memcpy ( m_ambientColor,newColor,sizeof ( float ) *3 );
    }
    void setSpecularColor ( const float* newColor )
    {
        memcpy ( m_specularColor,newColor,sizeof ( float ) *3 );
    }
    void setShininess ( const float newShininess )
    {
        m_shininess = newShininess;
    }
    void setSpecularExponent ( const float newExponent )
    {
        m_specularExponent = newExponent;
    }
    void setOpacity ( const float newOpacity )
    {
        m_opacity = newOpacity;
    }
    void setTwoSided ( const bool isTwoSided )
    {
        m_twoSided = isTwoSided;
    }

};

///////////////////////////////////////////////////////
// 3DS Mesh
//  Contains all vertex, face, texcoord & normal data
///////////////////////////////////////////////////////

class mesh3DS
{
private:
    std::string m_name;
    std::string m_materialName;
    RModel3DS *m_parentModel;

    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<ushort> m_faces;
    std::vector<unsigned int> m_faceSmoothing;
    std::vector<float> m_texcoords;
    std::map<std::string, std::vector<ushort> > m_materialFaces;

    //VBO IDs
    GLuint m_verticesArrayId;
    GLuint m_normalsArrayId;
    GLuint m_texcoordsArrayId;

    unsigned int m_drawMode;

    double m_Sines[314];
    bool m_wave;
    int m_waveTick;

public:

    mesh3DS ( RModel3DS *parentModel ) :m_parentModel ( parentModel )
    {
        m_wave = false;
    }

    void buildMesh();
    void calculateNormals();
    void sortFacesByMaterial();
    void draw ( int textureID = 0, bool pregenetateList = false );
    void setWaved ( bool wave )
    {
        m_wave = wave;
        m_waveTick = 500;
        for ( int a=0;a < 314;a++ ) m_Sines[a] = sin ( ( ( double ) a*2 ) /10 ) /6;
    }

    void addVertex ( const float vertex )
    {
        m_vertices.push_back ( vertex );
    }
    void addFaceIndex ( const ushort faceIndex )
    {
        m_faces.push_back ( faceIndex );
    }
    void addMaterialFace ( const std::string &name, const ushort faceIndex )
    {
        m_materialFaces[name].push_back ( faceIndex );
    }
    void addFaceSmoothing ( const unsigned int smoothing )
    {
        m_faceSmoothing.push_back ( smoothing );
    }
    void addTexcoord ( const float texcoord )
    {
        m_texcoords.push_back ( texcoord );
    }

    std::string getMaterial() const
    {
        return m_materialName;
    }
    std::string getName() const
    {
        return m_name;
    }
    int getNumFaces() const
    {
        return ( int ) m_faces.size();
    }

    void setMaterial ( const std::string &newMat )
    {
        m_materialName = newMat;
    }
    void setName ( const std::string newName )
    {
        m_name = newName;
    }
    void setDrawMode ( const unsigned int newDrawMode )
    {
        m_drawMode = newDrawMode;
    }


};

//////////////////////////////////////////////////////////////////
// 3DS Model
//  Contains all the meshes & materials that make up a 3DS model
//////////////////////////////////////////////////////////////////

typedef struct
{
    float minX,maxX;
    float minY,maxY;
    float minZ,maxZ;
} boundingBox3DS;

class RModel3DS
{
private:
    RResourceDB * m_rsc;
    std::string m_filename;
    std::string m_filepath;
    std::vector<mesh3DS> m_meshes;
    std::map<std::string, material3DS> m_materials;
    unsigned int m_drawMode;
    boundingBox3DS m_boundingBox;
    float m_scale;
    float m_centerX, m_centerY, m_centerZ;
    bool listsArePregenerated ;

    // temporaries used while reading chunks
    mesh3DS *m_currentMesh;
    material3DS *m_currentMaterial;
    std::vector<material3DS> *m_currentMaterialFaces;
    float m_currentColor[3];
    ushort m_tempUshort;
    float m_tempFloat;
    std::string m_tempString;

    void readChunk ( std::ifstream *modelFile, const int objectStart, const int objectLength );

    // Private copy and assignment constructors
    // to prevent object being copied
    // Private copy and assignment constructors
    // to prevent object being copied
    RModel3DS ( const RModel3DS &model );
    RModel3DS &operator= ( const RModel3DS &model );

    void load();

public:
    RModel3DS ( const char* filename, float scale = 1, RResourceDB * resources = 0 );

    void draw ( int TextureID = 0, bool centruj = false );
    void setWaved ( bool wave );
    std::string getFilename()
    {
        return m_filename;
    }
    material3DS getMaterial ( const std::string &matName )
    {
        std::map<std::string, material3DS>::const_iterator cit = m_materials.find(matName);
        if(cit == m_materials.end())
        {
            material3DS mat(m_rsc);
            m_materials.insert(std::pair<std::string, material3DS>(matName, material3DS(m_rsc)));
            return mat;
        }
        else
        {
            return cit->second;
        }
    }
};

} // namespace editor

#endif //MODEL3DS_H