/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "dieselMixture.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

template<class ThermoType>
const char* Foam::dieselMixture<ThermoType>::specieNames_[2] = {"ft", "fu"};


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::dieselMixture<ThermoType>::dieselMixture
(
    const dictionary& thermoDict,
    const fvMesh& mesh
)
:
    basicMultiComponentMixture
    (
        thermoDict,
        speciesTable(nSpecies_, specieNames_),
        mesh
    ),

    stoicRatio_(thermoDict.lookup("stoichiometricAirFuelMassRatio")),

    fuel_(thermoDict.lookup("fuel")),
    oxidant_(thermoDict.lookup("oxidant")),
    products_(thermoDict.lookup("burntProducts")),

    mixture_("mixture", fuel_),

    ft_(Y("ft")),
    fu_(Y("fu"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class ThermoType>
const ThermoType& Foam::dieselMixture<ThermoType>::mixture
(
    const scalar ft,
    const scalar fu
) const
{
    if (ft < 0.0001)
    {
        return oxidant_;
    }
    else
    {
        scalar ox = 1 - ft - (ft - fu)*stoicRatio().value();
        scalar pr = 1 - fu - ox;

        mixture_ = fu/fuel_.W()*fuel_;
        mixture_ += ox/oxidant_.W()*oxidant_;
        mixture_ += pr/products_.W()*products_;

        return mixture_;
    }
}


template<class ThermoType>
void Foam::dieselMixture<ThermoType>::read(const dictionary& thermoDict)
{
    fuel_ = ThermoType(thermoDict.lookup("fuel"));
    oxidant_ = ThermoType(thermoDict.lookup("oxidant"));
    products_ = ThermoType(thermoDict.lookup("burntProducts"));
}


// ************************************************************************* //
