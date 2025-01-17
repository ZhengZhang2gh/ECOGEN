//  
//       ,---.     ,--,    .---.     ,--,    ,---.    .-. .-. 
//       | .-'   .' .')   / .-. )  .' .'     | .-'    |  \| | 
//       | `-.   |  |(_)  | | |(_) |  |  __  | `-.    |   | | 
//       | .-'   \  \     | | | |  \  \ ( _) | .-'    | |\  | 
//       |  `--.  \  `-.  \ `-' /   \  `-) ) |  `--.  | | |)| 
//       /( __.'   \____\  )---'    )\____/  /( __.'  /(  (_) 
//      (__)              (_)      (__)     (__)     (__)     
//      Official webSite: https://code-mphi.github.io/ECOGEN/
//
//  This file is part of ECOGEN.
//
//  ECOGEN is the legal property of its developers, whose names 
//  are listed in the copyright file included with this source 
//  distribution.
//
//  ECOGEN is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published 
//  by the Free Software Foundation, either version 3 of the License, 
//  or (at your option) any later version.
//  
//  ECOGEN is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with ECOGEN (file LICENSE).  
//  If not, see <http://www.gnu.org/licenses/>.

#ifndef ELEMENTPRISM_H
#define ELEMENTPRISM_H

#include "ElementNS.h"
#include "FaceTriangle.h"
#include "FaceQuadrangle.h"

class ElementPrism : public ElementNS
{
public:
  ElementPrism();
  virtual ~ElementPrism();

  virtual void construitFaces(const Coord* noeuds, FaceNS** faces, int& indexMaxFaces, int** facesBuff, int* sumNodesBuff); 
  virtual void construitFacesSimplifie(int& iMax, int** facesBuff, int* sumNodesBuff);
  virtual void attributFaceCommunicante(FaceNS** faces, const int& indexMaxFaces, const int& numberNoeudsInternes);
  virtual int compteFaceCommunicante(std::vector<int*>& faces, std::vector<int>& sumNodesBuff);
  virtual int compteFaceCommunicante(int& iMax, int** faces, int* sumNodesBuff);

private:
  virtual void computeVolume(const Coord* noeuds);
  virtual void computeLCFL(const Coord* noeuds);

  static const int TYPEGMSH;
  static const int NUMBERNODES;
  static const int NUMBERFACES; /* Here the are 3 quadrangles and 2 triangles */
  static const int TYPEVTK;
};

#endif // ELEMENTPRISM_H