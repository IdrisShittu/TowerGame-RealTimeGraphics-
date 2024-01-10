#include "ConfigurationFileLoader.h"


ConfigurationFileLoader::ConfigurationFileLoader(const char* const configurationFileName): m_cameraPosition(XMFLOAT3()), m_terrainDimensions(XMFLOAT3()), m_terrainScale(XMFLOAT3()), m_rocketPosition(XMFLOAT3()), m_rocketRotation(XMFLOAT3()), m_rocketScale(XMFLOAT3()), m_sunAmbient(XMFLOAT4()), m_sunDiffuse(XMFLOAT4()), m_sunSpecular(XMFLOAT4()), m_sunSpecularPower(0.0f), m_moonAmbient(XMFLOAT4()), m_moonDiffuse(XMFLOAT4()), m_moonSpecular(XMFLOAT4()), m_moonSpecularPower(0.0f), m_launchPadScale(XMFLOAT3()), m_launchPadTessellationValues(XMFLOAT4()), m_launchPadDisplacementValues(XMFLOAT4()), m_antTweakBarConfigData(nullptr)
{
	ifstream fin;

	//Open configuration file
	fin.open(configurationFileName);

	if (fin.fail())
	{
		return;
	}

	char cmd[256] = { 0 };

	while (!fin.eof())
	{
		float x, y, z, w;

		fin >> cmd;

		if (0 == strcmp(cmd, "CameraPosition"))
		{
			fin >> x >> y >> z;
			m_cameraPosition = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "TerrainDimensions"))
		{
			fin >> x >> y >> z;
			m_terrainDimensions = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "TerrainScale"))
		{
			fin >> x >> y >> z;
			m_terrainScale = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "RocketPosition"))
		{
			fin >> x >> y >> z;
			m_rocketPosition = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "RocketRotation"))
		{
			fin >> x >> y >> z;
			m_rocketRotation = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "RocketScale"))
		{
			fin >> x >> y >> z;
			m_rocketScale = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "SunAmbient"))
		{
			fin >> x >> y >> z >> w;
			m_sunAmbient = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "SunDiffuse"))
		{
			fin >> x >> y >> z >> w;
			m_sunDiffuse = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "SunSpecular"))
		{
			fin >> x >> y >> z >> w;
			m_sunSpecular = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "SunSpecularPower"))
		{
			fin >> x;
			m_sunSpecularPower = x;
		}
		else if (0 == strcmp(cmd, "MoonAmbient"))
		{
			fin >> x >> y >> z >> w;
			m_moonAmbient = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "MoonDiffuse"))
		{
			fin >> x >> y >> z >> w;
			m_moonDiffuse = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "MoonSpecular"))
		{
			fin >> x >> y >> z >> w;
			m_moonSpecular = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "MoonSpecularPower"))
		{
			fin >> x;
			m_moonSpecularPower = x;
		}
		else if (0 == strcmp(cmd, "LaunchPadScale"))
		{
			fin >> x >> y >> z;
			m_launchPadScale = XMFLOAT3(x, y, z);
		}
		else if (0 == strcmp(cmd, "LaunchPadTessellationValues"))
		{
			fin >> x >> y >> z >> w;
			m_launchPadTessellationValues = XMFLOAT4(x, y, z, w);
		}
		else if (0 == strcmp(cmd, "LaunchPadDisplacementValues"))
		{
			fin >> x >> y >> z >> w;
			m_launchPadDisplacementValues = XMFLOAT4(x, y, z, w);
		}
	}

	m_antTweakBarConfigData = TwNewBar("Configuration");
	TwDefine(" Configuration label='Configuration File Data' position='1120 20' size='220 320'");
	TwAddVarRW(m_antTweakBarConfigData, "CameraPosition: ", TW_TYPE_DIR3F, &m_cameraPosition, "");
	TwAddVarRW(m_antTweakBarConfigData, "TerrainDim: ", TW_TYPE_DIR3F, &m_terrainDimensions, "");
	TwAddVarRW(m_antTweakBarConfigData, "TerrainScale: ", TW_TYPE_DIR3F, &m_terrainScale, "");
	TwAddVarRW(m_antTweakBarConfigData, "RocketPosition: ", TW_TYPE_DIR3F, &m_rocketPosition, "");
	TwAddVarRW(m_antTweakBarConfigData, "RocketRotation: ", TW_TYPE_DIR3F, &m_rocketRotation, "");
	TwAddVarRW(m_antTweakBarConfigData, "RocketScale: ", TW_TYPE_DIR3F, &m_rocketScale, "");
	TwAddVarRW(m_antTweakBarConfigData, "RocketRotation: ", TW_TYPE_DIR3F, &m_rocketRotation, "");
	TwAddVarRW(m_antTweakBarConfigData, "SunAmbient: ", TW_TYPE_COLOR4F, &m_sunAmbient, "");
	TwAddVarRW(m_antTweakBarConfigData, "SunDiffuse: ", TW_TYPE_COLOR4F, &m_sunDiffuse, "");
	TwAddVarRW(m_antTweakBarConfigData, "SunSpecular: ", TW_TYPE_COLOR4F, &m_sunSpecular, "");
	TwAddVarRW(m_antTweakBarConfigData, "SunSpecularPower: ", TW_TYPE_FLOAT, &m_sunSpecularPower, "");
	TwAddVarRW(m_antTweakBarConfigData, "MoonAmbient: ", TW_TYPE_COLOR4F, &m_moonAmbient, "");
	TwAddVarRW(m_antTweakBarConfigData, "MoonDiffuse: ", TW_TYPE_COLOR4F, &m_moonDiffuse, "");
	TwAddVarRW(m_antTweakBarConfigData, "MoonSpecular: ", TW_TYPE_COLOR4F, &m_moonSpecular, "");
	TwAddVarRW(m_antTweakBarConfigData, "MoonSpecularPower: ", TW_TYPE_FLOAT, &m_moonSpecularPower, "");
	TwAddVarRW(m_antTweakBarConfigData, "LaunchPadScale: ", TW_TYPE_DIR3F, &m_launchPadScale, "");
	TwAddVarRW(m_antTweakBarConfigData, "LaunchPadTess: ", TW_TYPE_QUAT4F, &m_launchPadTessellationValues, "");
	TwAddVarRW(m_antTweakBarConfigData, "LaunchPadDisp: ", TW_TYPE_QUAT4F, &m_launchPadDisplacementValues, "");
}

ConfigurationFileLoader::~ConfigurationFileLoader()
{
	TwTerminate();
}

const XMFLOAT3& ConfigurationFileLoader::GetCameraPosition() const
{
	return m_cameraPosition;
}

const XMFLOAT3& ConfigurationFileLoader::GetTerrainDimensions() const
{
	return m_terrainDimensions;
}

const XMFLOAT3& ConfigurationFileLoader::GetTerrainScale() const
{
	return m_terrainScale;
}

const XMFLOAT3& ConfigurationFileLoader::GetRocketPosition() const
{
	return m_rocketPosition;
}

const XMFLOAT3& ConfigurationFileLoader::GetRocketRotation() const
{
	return m_rocketRotation;
}

const XMFLOAT3& ConfigurationFileLoader::GetRocketScale() const
{
	return m_rocketScale;
}

const XMFLOAT4& ConfigurationFileLoader::GetSunAmbient() const
{
	return m_sunAmbient;
}

const XMFLOAT4& ConfigurationFileLoader::GetSunDiffuse() const
{
	return m_sunDiffuse;
}

const XMFLOAT4& ConfigurationFileLoader::GetSunSpecular() const
{
	return m_sunSpecular;
}

const float ConfigurationFileLoader::GetSunSpecularPower() const
{
	return m_sunSpecularPower;
}

const XMFLOAT4& ConfigurationFileLoader::GetMoonAmbient() const
{
	return m_moonAmbient;
}

const XMFLOAT4& ConfigurationFileLoader::GetMoonDiffuse() const
{
	return m_moonDiffuse;
}

const XMFLOAT4& ConfigurationFileLoader::GetMoonSpecular() const
{
	return m_moonSpecular;
}

const float ConfigurationFileLoader::GetMoonSpecularPower() const
{
	return m_moonSpecularPower;
}

const XMFLOAT3& ConfigurationFileLoader::GetLaunchPadScale() const
{
	return m_launchPadScale;
}

const XMFLOAT4& ConfigurationFileLoader::GetLaunchPadTessellationValues() const
{
	return m_launchPadTessellationValues;
}

const XMFLOAT4& ConfigurationFileLoader::GetLaunchPadDisplacementValues() const
{
	return m_launchPadDisplacementValues;
}







