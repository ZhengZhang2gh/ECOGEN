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

#include "AddPhys.h"

//***********************************************************************

AddPhys::AddPhys(){}

//***********************************************************************

AddPhys::~AddPhys(){}

//***********************************************************************

void AddPhys::computeFluxAddPhys(CellInterface* cellInterface)
{
  this->solveFluxAddPhys(cellInterface);

  if (cellInterface->getCellGauche()->getLvl() == cellInterface->getCellDroite()->getLvl()) {     //CoefAMR = 1 for the two
    this->addFluxAddPhys(cellInterface, 1.);                                        //Add flux on the right cell
    this->subtractFluxAddPhys(cellInterface, 1.);                                   //Subtract flux on the left cell
  }
  else if (cellInterface->getCellGauche()->getLvl() > cellInterface->getCellDroite()->getLvl()) { //CoefAMR = 1 for the left and 0.5 for the right
    this->addFluxAddPhys(cellInterface, 0.5);                                       //Add flux on the right cell
    this->subtractFluxAddPhys(cellInterface, 1.);                                   //Subtract flux on the left cell
  }
  else {                                                                                          //CoefAMR = 0.5 for the left and 1 for the right
    this->addFluxAddPhys(cellInterface, 1.);                                        //Add flux on the right cell
    this->subtractFluxAddPhys(cellInterface, 0.5);                                  //Subtract flux on the left cell
  }
}

//***********************************************************************

void AddPhys::computeFluxAddPhysBoundary(CellInterface* cellInterface)
{
  this->solveFluxAddPhysBoundary(cellInterface);
  this->subtractFluxAddPhys(cellInterface, 1.); //Subtract flux on the left cell
}

//***********************************************************************

void AddPhys::addFluxAddPhys(CellInterface* cellInterface, const double& coefAMR)
{
  //No "time step"
  double coefA = cellInterface->getFace()->getSurface() / cellInterface->getCellDroite()->getElement()->getVolume() * coefAMR;
  cellInterface->getCellDroite()->getCons()->addFlux(coefA);
}

//***********************************************************************

void AddPhys::subtractFluxAddPhys(CellInterface* cellInterface, const double& coefAMR)
{
  //No "time step"
  double coefA = cellInterface->getFace()->getSurface() / cellInterface->getCellGauche()->getElement()->getVolume() * coefAMR;
  cellInterface->getCellGauche()->getCons()->subtractFlux(coefA);
}

//***********************************************************************

void AddPhys::addNonConsAddPhys(Cell* cell)
{
  this->addNonCons(cell);
}

//***********************************************************************
