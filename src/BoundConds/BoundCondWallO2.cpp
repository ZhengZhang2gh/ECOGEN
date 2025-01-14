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

#include "BoundCondWallO2.h"

//****************************************************************************

BoundCondWallO2::BoundCondWallO2(const BoundCondWallO2& Source, const int& lvl) : BoundCondWall(Source, lvl)
{}

//****************************************************************************

BoundCondWallO2::BoundCondWallO2(int numPhysique, tinyxml2::XMLElement* element, std::string fileName) : BoundCondWall(numPhysique, element, fileName)
{}

//****************************************************************************

BoundCondWallO2::BoundCondWallO2(int numPhysique) : BoundCondWall(numPhysique)
{}

//****************************************************************************

BoundCondWallO2::~BoundCondWallO2()
{
  for (int k = 0; k < numberPhases; k++) {
    delete m_vecPhasesSlopes[k];
  }
  delete[] m_vecPhasesSlopes;
  delete m_mixtureSlopes;
  delete[] m_vecTransportsSlopes;
}

//****************************************************************************

void BoundCondWallO2::createBoundary(TypeMeshContainer<CellInterface*>& cellInterfaces)
{
  cellInterfaces.push_back(new BoundCondWallO2(*(this)));
}

//***********************************************************************

void BoundCondWallO2::allocateSlopes(int& /*allocateSlopeLocal*/)
{
  //Allocation of phase slopes
  m_vecPhasesSlopes = new Phase*[numberPhases];
  //On attribut les phases a partir de la cell a gauche (car cell a droite inexistante pour les limites)
  //Necessaire car il faut connaitre le type de phase (ex: PhasePUEq, etc.))
  //Ensuite on met a zero toutes les slopes
  for (int k = 0; k < numberPhases; k++) {
    m_cellLeft->getPhase(k)->allocateAndCopyPhase(&m_vecPhasesSlopes[k]);
    m_vecPhasesSlopes[k]->setToZero();
  }
  m_cellLeft->getMixture()->allocateAndCopyMixture(&m_mixtureSlopes);
  m_mixtureSlopes->setToZero();

  //Allocation des slopes sur transports
  m_vecTransportsSlopes = new Transport[numberTransports];
  for (int k = 0; k < numberTransports; k++) {
    m_vecTransportsSlopes[k].setValue(0.);
  }
}

//***********************************************************************

void BoundCondWallO2::computeSlopes(Prim type)
{
  //Slopes des velocities normals aux limites
  double distanceX, distanceY, distanceZ;
  distanceX = m_cellLeft->getElement()->distanceX(m_face);
  distanceY = m_cellLeft->getElement()->distanceY(m_face);
  distanceZ = m_cellLeft->getElement()->distanceZ(m_face);
  if (std::fabs(distanceX) > 1.e-8) {
    for (int k = 0; k < numberPhases; k++) {
      m_vecPhasesSlopes[k]->setU(m_cellLeft->getPhase(k, type)->getVelocity().getX() / distanceX);
    }
    m_mixtureSlopes->setU(m_cellLeft->getMixture(type)->getVelocity().getX() / distanceX);
  }
  if (std::fabs(distanceY) > 1.e-8) {
    for (int k = 0; k < numberPhases; k++) {
      m_vecPhasesSlopes[k]->setV(m_cellLeft->getPhase(k, type)->getVelocity().getY() / distanceY);
    }
    m_mixtureSlopes->setV(m_cellLeft->getMixture(type)->getVelocity().getY() / distanceY);
  }
  if (std::fabs(distanceZ) > 1.e-8) {
    for (int k = 0; k < numberPhases; k++) {
      m_vecPhasesSlopes[k]->setW(m_cellLeft->getPhase(k, type)->getVelocity().getZ() / distanceZ);
    }
    m_mixtureSlopes->setW(m_cellLeft->getMixture(type)->getVelocity().getZ() / distanceZ);
  }
}

//***********************************************************************

void BoundCondWallO2::solveRiemann(double& dtMax, Limiter& globalLimiter, Limiter& interfaceLimiter, Limiter& globalVolumeFractionLimiter, Limiter& interfaceVolumeFractionLimiter, Prim type)
{
  bufferCellLeft->copyVec(m_cellLeft->getPhases(type), m_cellLeft->getMixture(type), m_cellLeft->getTransports(type));

  //Calcul des distances cell interfaces <-> cells pour l extrapolation
  double distanceGauche(this->distance(m_cellLeft));
  //KS//FP// A voir avec Fabien comment faire ca bien pour qu'il n'y est aucun probleme en non-structure !
  //Probleme que la distance est une norme et donc on ne change pas le signe de la slope pour faire correctement l'extrapolation
  if (m_face->getNormal().getX() < 0. || m_face->getNormal().getY() < 0. || m_face->getNormal().getZ() < 0.) { distanceGauche = -distanceGauche; }

  //Extrapolation gauche
  double epsInterface(1.e-4);
  m_cellLeft->computeLocalSlopesLimite(*this, globalLimiter, interfaceLimiter, globalVolumeFractionLimiter, interfaceVolumeFractionLimiter, epsInterface);
  for (int k = 0; k < numberPhases; k++) {
    bufferCellLeft->getPhase(k)->extrapolate(*slopesPhasesLocal1[k], distanceGauche);
  }
  bufferCellLeft->getMixture()->extrapolate(*slopesMixtureLocal1, distanceGauche);
  for (int k = 0; k < numberTransports; k++) {
    bufferCellLeft->getTransport(k).extrapolate(slopesTransportLocal1[k], distanceGauche);
  }

  //Projection des velocities sur repere attache a la face
  bufferCellLeft->localProjection(m_face->getNormal(), m_face->getTangent(), m_face->getBinormal());
  //Calcul des variables etendus (Phases, Mixture, AddPhys)
  bufferCellLeft->fulfillState();

  //Riemann problem
  double dxLeft(m_cellLeft->getElement()->getLCFL());
  dxLeft = dxLeft*std::pow(2., (double)m_lvl);
  this->solveRiemannBoundary(*bufferCellLeft, dxLeft, dtMax);
  //Handling of transport functions (m_Sm known: need to be called after Riemann solver)
  if (numberTransports > 0) { this->solveRiemannTransportBoundary(*bufferCellLeft); }

  //Flux projection on absolute reference frame
  model->reverseProjection(m_face->getNormal(), m_face->getTangent(), m_face->getBinormal());
}

//***********************************************************************

Phase* BoundCondWallO2::getSlopesPhase(const int& phaseNumber) const
{
  return m_vecPhasesSlopes[phaseNumber];
}

//***********************************************************************

Mixture* BoundCondWallO2::getSlopesMixture() const
{
  return m_mixtureSlopes;
}

//***********************************************************************

Transport* BoundCondWallO2::getSlopesTransport(const int& numberTransport) const
{
  return &m_vecTransportsSlopes[numberTransport];
}


//***************************************************************************
//******************************AMR Method***********************************
//***************************************************************************

void BoundCondWallO2::creerCellInterfaceChild()
{
  m_cellInterfacesChildren.push_back(new BoundCondWallO2(*this, m_lvl + 1));
}

//****************************************************************************