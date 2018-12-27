#include "stdafx.h"
#include "CppUnitTest.h"
#include <CAN_Serializer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CanToSerialUno_Test
{		
	TEST_CLASS(CanSerializer_Test)
	{
	public:
		
		TEST_METHOD(CanSerializerInit_IsTrueTest)
		{
			// TODO: Your test code here
			CAN_Serializer serializer = CAN_Serializer();
			serializer.init(48);
		}

	};
}