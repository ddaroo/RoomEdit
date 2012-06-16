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

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include "RModel3DS.h"
#include "Vector.h"
#include "RConfig.h"
#include "RResourceDB.h"
#include "global.h"

namespace reditor
{
    
model3DS::model3DS(const char* filename,const float scale) : m_filename(filename), m_scale(scale) {

    std::ifstream *modelFile = new std::ifstream(filename,std::ios::in | std::ios::binary | std::ios::ate);
    QFileInfo finfo(filename);

    if (!modelFile->is_open()) {
        qCritical() << "Could not open " << finfo.absoluteFilePath();
        return;
    }
    if (int(modelFile->tellg()) == 0) {
        qCritical() << "Model " << finfo.absoluteFilePath() << "is empty";
        modelFile->close();
        return;
    }

    // Extract path from filename
    int lastSlashPosition=-1, lastForwardSlash=-1, lastBackslash=-1;
    lastForwardSlash = (int)m_filename.find_last_of('/');
    lastBackslash = (int)m_filename.find_last_of('\\');
    if (lastForwardSlash > lastSlashPosition) lastSlashPosition = lastForwardSlash;
    if (lastBackslash > lastSlashPosition) lastSlashPosition = lastBackslash;
    m_filepath = m_filename.substr(0,lastSlashPosition+1);
    m_filename = m_filename.substr(lastSlashPosition+1);

    // Check to make sure file is valid 3DS format (begins with 0x4D4D)
    ushort chunkHeader;
    unsigned int chunkLength;

    modelFile->seekg(0, std::ios::beg);
    modelFile->read((char*)&chunkHeader,2);
    modelFile->read((char*)&chunkLength,4);

    if (chunkHeader != CHUNK_MAIN) {
        qCritical() << "Model " << finfo.absoluteFilePath() << " is not a valid 3DS file";
        modelFile->close();
        return;
    }

    m_drawMode = DRAW_IMMEDIATE_MODE;//DRAW_VERTEX_ARRAY;

    // Initialise bounding box to min & max 4-byte float values
    m_boundingBox.minX = m_boundingBox.minY = m_boundingBox.minZ = 3.4e+38f;
    m_boundingBox.maxX = m_boundingBox.maxY = m_boundingBox.maxZ = 3.4e-38f;

    // Read all 3DS chunks recursively
    while (!modelFile->eof()) {
        readChunk(modelFile, modelFile->tellg(), chunkLength);
    }

    m_centerX = (m_boundingBox.minX + m_boundingBox.maxX) / 2.f;
    m_centerY = (m_boundingBox.minY + m_boundingBox.maxY) / 2.f;
    m_centerZ = (m_boundingBox.minZ + m_boundingBox.maxZ) / 2.f;

    // Model loaded, clean up
    modelFile->close();
    delete modelFile;
    listsArePregenerated = false;
}

void model3DS::readChunk(std::ifstream *modelFile,  int objectStart, int objectLength) {
    //std::cout<<std::hex<<"readChunk("<<objectStart<<"-"<<(objectStart+objectLength)<<")"<<std::dec<<std::endl;

    ushort chunkHeader;
    unsigned int chunkLength;

    unsigned long offset;
    ushort numVertices;
    ushort usTemp;
    unsigned int uiTemp;
    float vertexX,vertexY,vertexZ;
    int v;
    std::string name;
    char currentLetter;
    unsigned char rgbByte;

    while ((modelFile->tellg() < (objectStart + objectLength)) && !modelFile->eof()) {

        offset = modelFile->tellg();

        modelFile->read((char*)&chunkHeader, 2);
        modelFile->read((char*)&chunkLength, 4);

        if (DEBUG_OUTPUT) std::cout<<std::hex<<"["<<offset<<"] chunk: 0x"<<chunkHeader<<" ("<<offset<<"-"<<(offset+chunkLength)<<")"<<std::dec<<std::endl;
        switch (chunkHeader) {

            //////////////////
            // Main chunks
            /////////////////

        case CHUNK_MAIN:
            continue;

        case CHUNK_3D_EDITOR:
            continue;

        case CHUNK_OBJECT_BLOCK:
            if (DEBUG_OUTPUT) std::cout<<std::endl<<"[Object block]"<<std::endl;

            m_currentMesh = new mesh3DS(this);
            m_currentMesh->setDrawMode(m_drawMode);

            // Read object name
            do {
                modelFile->read(&currentLetter,1);
                name += currentLetter;
            } while (currentLetter!='\0' && name.length()<20);
            m_currentMesh->setName(name);
            if (DEBUG_OUTPUT) std::cout<<"  Object: "<<name<<std::endl;
            name.erase();

            // Read object sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));

            if (m_currentMesh->getNumFaces() != 0) {
                m_currentMesh->buildMesh();
                m_meshes.push_back(*m_currentMesh);
            }
            delete m_currentMesh;
            break;

            /////////////////////
            // Geometry chunks
            ////////////////////

        case CHUNK_MESH:
            continue;

        case CHUNK_VERTICES:
            modelFile->read((char*)&numVertices,2);
            for (v=0; v < numVertices*3; v+=3) {
                modelFile->read((char*)&vertexX,4);
                modelFile->read((char*)&vertexY,4);
                modelFile->read((char*)&vertexZ,4);
                // 3DS Max has different axes to OpenGL
                vertexX *= m_scale;
                vertexY *= m_scale;
                vertexZ *= m_scale;
                m_currentMesh->addVertex(vertexX);// x
                m_currentMesh->addVertex(vertexZ);// y
                m_currentMesh->addVertex(-vertexY);// z
                // Update bounding box
                if (vertexX < m_boundingBox.minX)m_boundingBox.minX = vertexX;
                if (vertexZ < m_boundingBox.minY)m_boundingBox.minY = vertexZ;
                if (-vertexY < m_boundingBox.minZ)m_boundingBox.minZ = -vertexY;
                if (vertexX > m_boundingBox.maxX)m_boundingBox.maxX = vertexX;
                if (vertexZ > m_boundingBox.maxY)m_boundingBox.maxY = vertexZ;
                if (-vertexY > m_boundingBox.maxZ)m_boundingBox.maxZ = -vertexY;
            }
            break;

        case CHUNK_TEXCOORDS: // texcoords list
            modelFile->read((char*)&numVertices,2);
            for (v=0; v < numVertices*2; v+=2) {
                modelFile->read((char*)&vertexX,4);
                modelFile->read((char*)&vertexY,4);
                m_currentMesh->addTexcoord(vertexX);
                m_currentMesh->addTexcoord(vertexY);
            }
            break;

        case CHUNK_FACES:
            modelFile->read((char*)&m_tempUshort,2);

            for (v=0; v < m_tempUshort*3; v+=3) {
                modelFile->read((char*)&usTemp,2);
                m_currentMesh->addFaceIndex(usTemp);
                modelFile->read((char*)&usTemp,2);
                m_currentMesh->addFaceIndex(usTemp);
                modelFile->read((char*)&usTemp,2);
                m_currentMesh->addFaceIndex(usTemp);
                modelFile->read((char*)&usTemp,2); //face flags
            }

            // Read face sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            break;

        case CHUNK_SMOOTHING_GROUP:
            for (v=0; v < m_tempUshort; v++) {
                modelFile->read((char*)&uiTemp,4);
                m_currentMesh->addFaceSmoothing(uiTemp);
                //if(DEBUG_OUTPUT) std::cout<<"Smoothing: "<<uiTemp<<std::endl;
            }
            break;

            /////////////////////
            // Material chunks
            ////////////////////

        case CHUNK_FACE_MATERIAL:
            // Read material name
            do {
                modelFile->read(&currentLetter,1);
                name += currentLetter;
            } while (currentLetter!='\0' && name.length()<20);

            modelFile->read((char*)&m_tempUshort,2);

            for (v=0; v < m_tempUshort; v++) {
                modelFile->read((char*)&usTemp,2);
                m_currentMesh->addMaterialFace(name, usTemp);
            }

            name.erase();

            break;

        case CHUNK_MATERIAL_BLOCK:
            if (DEBUG_OUTPUT) std::cout<<std::endl<<"[Material block]"<<std::endl;

            m_currentMaterial = new material3DS();
            m_currentMaterial->setList(glGenLists(1));
            // Read material sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));

            m_materials[m_currentMaterial->getName()] = *m_currentMaterial;
            delete m_currentMaterial;
            break;

        case CHUNK_MATERIAL_NAME:
            // Read material name and add to current material
            do {
                modelFile->read(&currentLetter,1);
                name += currentLetter;
            } while (currentLetter!='\0' && name.length()<20);
            m_currentMaterial->setName(name);
            if (DEBUG_OUTPUT) std::cout<<"  Material: "<<m_currentMaterial->getName()<<"("<<m_currentMaterial->getName().size()<<")"<<std::endl;
            name.erase();
            break;

        case CHUNK_TEXTURE_MAP:
        case CHUNK_BUMP_MAP:
            //Read texture name and add to current material
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->loadTexture(m_tempString, chunkHeader);

            break;

        case CHUNK_MAP_FILENAME:
            // Read texture map filename
            m_tempString.erase();
            do {
                modelFile->read(&currentLetter,1);
                m_tempString += currentLetter;
            } while (currentLetter!='\0' && m_tempString.length()<20);
            break;

        case CHUNK_MATERIAL_TWO_SIDED:
            m_currentMaterial->setTwoSided(true);
            break;

        case CHUNK_DIFFUSE_COLOR:
            // Read color sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setDiffuseColor(m_currentColor);

            break;

        case CHUNK_AMBIENT_COLOR:
            // Read color sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setAmbientColor(m_currentColor);

            break;

        case CHUNK_SPECULAR_COLOR:
            // Read color sub-chunks
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setSpecularColor(m_currentColor);

            break;

        case CHUNK_SPECULAR_EXPONENT:
            // Read percent sub-chunk
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setSpecularExponent(m_tempFloat);
            break;

        case CHUNK_SHININESS:
            // Read percent sub-chunk
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setShininess(m_tempFloat);
            break;

        case CHUNK_TRANSPARENCY:
            // Read percent sub-chunk
            readChunk(modelFile, modelFile->tellg(), chunkLength - (long(modelFile->tellg()) - offset));
            m_currentMaterial->setOpacity(1.0f - m_tempFloat);
            break;

            /////////////////////
            // Global chunks
            ////////////////////

        case CHUNK_RGB_FLOAT:
        case CHUNK_RGB_FLOAT_GAMMA:
            modelFile->read((char*)&m_currentColor[0],4);
            modelFile->read((char*)&m_currentColor[1],4);
            modelFile->read((char*)&m_currentColor[2],4);
            break;

        case CHUNK_RGB_BYTE:
        case CHUNK_RGB_BYTE_GAMMA:
            modelFile->read((char*)&rgbByte,1);
            m_currentColor[0]=float(rgbByte)/255.f;
            modelFile->read((char*)&rgbByte,1);
            m_currentColor[1]=float(rgbByte)/255.f;
            modelFile->read((char*)&rgbByte,1);
            m_currentColor[2]=float(rgbByte)/255.f;
            break;

        case CHUNK_PERCENT_INT:
            modelFile->read((char*)&usTemp,2);
            m_tempFloat = usTemp / 100.f;
            break;

        case CHUNK_PERCENT_FLOAT:
            modelFile->read((char*)&m_tempFloat,4);
            m_tempFloat /= 100.f;
            break;

        default:
            break; // any other chunk

        }

        // Go to the next chunk's header (if any left in object)
        modelFile->seekg(offset + chunkLength, std::ios::beg);
    }

}

void material3DS::loadTexture(std::string filename, int chunkType) 
{
    QFileInfo finfo(QString::fromStdString(filename));
    switch (chunkType) {
    case CHUNK_TEXTURE_MAP:
        m_textureMapId = rscDB.texture(finfo.baseName())->id();
        m_hasTextureMap = true;
        break;
    case CHUNK_BUMP_MAP:
        m_bumpMapId = rscDB.texture(finfo.baseName())->id();
        m_hasBumpMap = true;
        break;
    }

}

void mesh3DS::buildMesh() {
    calculateNormals();
    sortFacesByMaterial();

}

void mesh3DS::calculateNormals() {

    // Doesn't take smoothing groups into account yet

    if (DEBUG_OUTPUT) std::cout<<"Calculating normals... ";
    m_normals.assign(m_vertices.size(), 0.0f);

    Vertex vtx1, vtx2, vtx3;
    Vector v1, v2, faceNormal;

    for (int face=0; face < int(m_faces.size()); face+=3) {
        // Calculate face normal
        vtx1.set(m_vertices[m_faces[face]*3], m_vertices[(m_faces[face]*3)+1], m_vertices[(m_faces[face]*3)+2]);
        vtx2.set(m_vertices[m_faces[face+1]*3], m_vertices[(m_faces[face+1]*3)+1], m_vertices[(m_faces[face+1]*3)+2]);
        vtx3.set(m_vertices[m_faces[face+2]*3], m_vertices[(m_faces[face+2]*3)+1], m_vertices[(m_faces[face+2]*3)+2]);

        v1 = vtx2 - vtx1;
        v2 = vtx3 - vtx1;

        faceNormal = v1.crossProduct(v2);

        // Add normal to all three vertex normals
        m_normals[m_faces[face]*3] += faceNormal.x;
        m_normals[(m_faces[face]*3)+1] += faceNormal.y;
        m_normals[(m_faces[face]*3)+2] += faceNormal.z;

        m_normals[m_faces[face+1]*3] += faceNormal.x;
        m_normals[(m_faces[face+1]*3)+1] += faceNormal.y;
        m_normals[(m_faces[face+1]*3)+2] += faceNormal.z;

        m_normals[m_faces[face+2]*3] += faceNormal.x;
        m_normals[(m_faces[face+2]*3)+1] += faceNormal.y;
        m_normals[(m_faces[face+2]*3)+2] += faceNormal.z;

    }

    //normalize all normals
    for (int n=0; n < int(m_normals.size()); n+=3) {
        faceNormal.set(m_normals[n], m_normals[n+1], m_normals[n+2]);
        faceNormal.normalize();
        m_normals[n] = faceNormal.x;
        m_normals[n+1] = faceNormal.y;
        m_normals[n+2] = faceNormal.z;
    }

    if (DEBUG_OUTPUT) std::cout<<"done"<<std::endl;
}

void mesh3DS::sortFacesByMaterial() {

    assert(getNumFaces()!=0);
    assert(m_parentModel!=NULL);

    std::vector<ushort> newMatFaces;

    // mark each face off as assigned to a material so
    // we can figure out which faces have no material
    std::vector<bool> assignedFaces;
    std::vector<bool>::iterator assignedFacesIter;
    assignedFaces.assign(m_faces.size() / 3, false);

    // loop over each material
    std::map<std::string, std::vector<ushort> >::iterator matFacesIter;
    for (matFacesIter=m_materialFaces.begin(); matFacesIter!=m_materialFaces.end(); ++matFacesIter) {
        //std::cout<<"    Faces in material '"<<matFacesIter->first<<"': "<<matFacesIter->second.size()<<std::endl;

        // loop over all the faces with that material
        std::vector<ushort>::iterator facesIter;
        for (facesIter=matFacesIter->second.begin(); facesIter!=matFacesIter->second.end(); ++facesIter) {
            newMatFaces.push_back(m_faces[((*facesIter)*3)]);
            newMatFaces.push_back(m_faces[((*facesIter)*3)+1]);
            newMatFaces.push_back(m_faces[((*facesIter)*3)+2]);
            assignedFaces[*facesIter]=true;
        }

        //replace the material's face indices with the actual face vertex indices
        m_materialFaces[matFacesIter->first].assign(newMatFaces.begin(),newMatFaces.end());
        newMatFaces.clear();
    }

    // Make a default material and assign any unused faces to it
    int numUnassignedFaces=0;
    for (assignedFacesIter=assignedFaces.begin(); assignedFacesIter!=assignedFaces.end(); ++assignedFacesIter) {
        if (*assignedFacesIter == false) {
            numUnassignedFaces++;
            //assign face to default material
        }
    }
    //std::cout<<"numUnassignedFaces: "<<numUnassignedFaces<<std::endl;
}

void mesh3DS::draw(int textureID, bool pregenetateList) {

    if (m_wave)
        ++m_waveTick;
    assert(getNumFaces()!=0);

    int face, numFaces, vertexIndex, texcoordIndex;
    GLuint materialFaces; //GL_FRONT or GL_FRONT_AND_BACK

    std::map<std::string, std::vector<ushort> >::iterator materialsIter;
    for (materialsIter=m_materialFaces.begin(); materialsIter!=m_materialFaces.end(); ++materialsIter) {
        const material3DS& currentMaterial = m_parentModel->getMaterial(materialsIter->first);

        // Bind texture map (if any)
        bool hasTextureMap = currentMaterial.hasTextureMap();
        if (textureID) glBindTexture(GL_TEXTURE_2D, textureID);
        else if (hasTextureMap) glBindTexture(GL_TEXTURE_2D, currentMaterial.getTextureMapId());
        else glBindTexture(GL_TEXTURE_2D, 0);

        const GLfloat *specular = currentMaterial.getSpecularColor();
        float shininess = currentMaterial.getShininess();
        float adjustedSpecular[4] = {specular[0]*shininess, specular[1]*shininess, specular[2]*shininess, 1};

        glPushAttrib(GL_LIGHTING_BIT);
        if (currentMaterial.isTwoSided()) {
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
            materialFaces = GL_FRONT_AND_BACK;
        }
        else {
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
            materialFaces = GL_FRONT;
        }

        // Apply material colors
        if (glIsEnabled(GL_LIGHTING)) {
            //GLfloat matZero[4]={0,0,0,0};

            GLfloat matOne[4]={1,1,1,1};
            if (hasTextureMap) { //replace color with texture, but keep lighting contribution
                glMaterialfv(materialFaces, GL_DIFFUSE, matOne);
            }
            else glMaterialfv(materialFaces, GL_DIFFUSE, currentMaterial.getDiffuseColor());
            glMaterialfv(materialFaces, GL_AMBIENT, currentMaterial.getAmbientColor());
            glMaterialfv(materialFaces, GL_SPECULAR, adjustedSpecular);
            glMaterialf(materialFaces, GL_SHININESS, 128.f * currentMaterial.getSpecularExponent());
        }
        else glColor3fv(currentMaterial.getDiffuseColor());


        std::vector<ushort> *currentMatFaces = &(materialsIter->second);
        numFaces = (int)currentMatFaces->size(); //number of faces in this material

        if (!pregenetateList)
            glCallList(currentMaterial.getList());  // uruchomienie rysowania ze swap'a
        else {

            //currentMaterial.setList(glGenLists(1));// wygenerowanie miejsca dla jednej listy i zwr�cenie do niej uchwytu - mozliwe jest generowanie szeregu list, pod kolejnymi (+1) wartosciami uchwytow, wtedy metoda zwraca uchwyt do pierwszej z nich
            glNewList(currentMaterial.getList(),GL_COMPILE); //definicja zawarto�ci listy("cube")

            switch (m_drawMode) {
            case DRAW_IMMEDIATE_MODE:

                glBegin(GL_TRIANGLES);
                for (face=0; face<numFaces; face+=3) {
                    if (hasTextureMap) {
                        texcoordIndex = (*currentMatFaces)[face]*2;
                        glTexCoord2f(m_texcoords[texcoordIndex], m_texcoords[texcoordIndex+1]);
                    }
                    vertexIndex = (*currentMatFaces)[face]*3;
                    glNormal3f(m_normals[vertexIndex], m_normals[vertexIndex+1], m_normals[vertexIndex+2]);
                    if (!m_wave)
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1], m_vertices[vertexIndex+2]);
                    else
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1]+m_Sines[(m_waveTick+(int)(abs(m_vertices[vertexIndex])*256)+(int)(abs(m_vertices[vertexIndex+2])*256))%314], m_vertices[vertexIndex+2]);
                    if (hasTextureMap) {
                        texcoordIndex = (*currentMatFaces)[face+1]*2;
                        glTexCoord2f(m_texcoords[texcoordIndex], m_texcoords[texcoordIndex+1]);
                    }
                    vertexIndex = (*currentMatFaces)[face+1]*3;
                    glNormal3f(m_normals[vertexIndex], m_normals[vertexIndex+1], m_normals[vertexIndex+2]);
                    if (!m_wave)
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1], m_vertices[vertexIndex+2]);
                    else
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1]+m_Sines[(m_waveTick+(int)(abs(m_vertices[vertexIndex])*256)+(int)(abs(m_vertices[vertexIndex+2])*256))%314], m_vertices[vertexIndex+2]);
                    if (hasTextureMap) {
                        texcoordIndex = (*currentMatFaces)[face+2]*2;
                        glTexCoord2f(m_texcoords[texcoordIndex], m_texcoords[texcoordIndex+1]);
                    }
                    vertexIndex = (*currentMatFaces)[face+2]*3;
                    glNormal3f(m_normals[vertexIndex], m_normals[vertexIndex+1], m_normals[vertexIndex+2]);
                    if (!m_wave)
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1], m_vertices[vertexIndex+2]);
                    else
                        glVertex3f(m_vertices[vertexIndex], m_vertices[vertexIndex+1]+m_Sines[(m_waveTick+(int)(abs(m_vertices[vertexIndex])*256)+(int)(abs(m_vertices[vertexIndex+2])*256))%314], m_vertices[vertexIndex+2]);

                }
                glEnd();

                break;

            case DRAW_VERTEX_ARRAY:

                glEnableClientState( GL_VERTEX_ARRAY );
                glEnableClientState( GL_NORMAL_ARRAY );

                if (hasTextureMap) {
                    glTexCoordPointer( 2, GL_FLOAT, 0, &m_texcoords[0] );
                    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
                }

                glVertexPointer( 3, GL_FLOAT, 0, &m_vertices[0] );
                glNormalPointer(GL_FLOAT, 0, &m_normals[0] );
                glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_SHORT, &(materialsIter->second[0]));

                glDisableClientState( GL_VERTEX_ARRAY );
                glDisableClientState( GL_NORMAL_ARRAY );
                if (hasTextureMap) {
                    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
                }

                break;

            case DRAW_VBO:

                break;

            default:
                qCritical() << "Invalid mesh draw mode specified";
                break;
            } // switch
            glEndList(); //koniec definicji zawarto�ci listy
        } // else po optymalizacji list
        glPopAttrib(); // GL_LIGHTING_BIT
    }
}

// rysuje wszystkie siatki, pobiera jako parametr:
// - int: id tekstury: je�li wi�ksze od 0 to wymusa automatyczne teksturowanie t� tekstur�, je�li 0 lub brak - przyjmuje teksturowanie z pliku
// - bool: centrowanie obiektu: je�li true lub brak: za pozycj� obiektu przymuje jego geometryczny �rodek, je�li false - przyjmje wsp��rz�dne z pliku
void model3DS::draw(int TextureID, bool centruj) {

    GLboolean texGenS;
    glGetBooleanv(GL_TEXTURE_GEN_S,&texGenS);
    GLboolean texGenT;
    glGetBooleanv(GL_TEXTURE_GEN_T,&texGenT);
    glShadeModel(GL_SMOOTH);                                        // cieniowanie

    if (TextureID) { //gdy podano niezerow� tekstur� nale�y w�aczy� automatyczne generowanie tekstur, w przeciwnym wypadku - koordynaty tekstury bazowej z modelu
        glEnable(GL_TEXTURE_GEN_S); // automatyczne generowanie koordynat
        glEnable(GL_TEXTURE_GEN_T);
    } else
    {
        glDisable(GL_TEXTURE_GEN_S); // automatyczne generowanie koordynat
        glDisable(GL_TEXTURE_GEN_T);
    }


    std::vector<mesh3DS>::iterator meshIter;

    glPushMatrix();
    if (centruj) glTranslatef(-m_centerX,-m_centerY,-m_centerZ);

    for (meshIter = m_meshes.begin(); meshIter != m_meshes.end(); meshIter++) {
        meshIter->draw(TextureID,!listsArePregenerated);
    }
    listsArePregenerated = true;
    glPopMatrix();


    if (texGenS == GL_TRUE) glEnable(GL_TEXTURE_GEN_S);
    else glDisable(GL_TEXTURE_GEN_S); // ustawienie poprzednich warto�ci
    if (texGenS == GL_TRUE) glEnable(GL_TEXTURE_GEN_T);
    else glDisable(GL_TEXTURE_GEN_T);

}

// ustala falowanie obiektu

void model3DS::setWaved(bool wave) {

    std::vector<mesh3DS>::iterator meshIter;

    for (meshIter = m_meshes.begin(); meshIter != m_meshes.end(); meshIter++)
        meshIter->setWaved (wave);

}

} // namespace editor