/*
 * commutator_types.hpp
 *
 *  Created on: Sep 15, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

enum class BldcStep
{
	AH_BL	= 0,
	AH_CL	= 1,
	BH_CL	= 2,
	BH_AL	= 3,
	CH_AL	= 4,
	CH_BL	= 5,
};

enum class Phase
{
	A,
	B,
	C
};

enum class PhaseMode
{
	Source,
	Sink
};
