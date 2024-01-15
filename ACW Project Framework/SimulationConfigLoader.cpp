#include "SimulationConfigLoader.h"
//

SimulationConfigLoader::SimulationConfigLoader(const char* const configurationFileName): 
    cameraPosition(XMFLOAT3()),
    terrainSize(XMFLOAT3()),
    terrainScale(XMFLOAT3()),
    rocketPosition(XMFLOAT3()),
    rocketRotation(XMFLOAT3()),
    rocketScale(XMFLOAT3()),
    sunlightAmbientColor(XMFLOAT4()),
    sunlightDiffuseColor(XMFLOAT4()),
    sunlightSpecularColor(XMFLOAT4()),
    sunlightSpecularIntensity(0.0f),
    moonlightAmbientColor(XMFLOAT4()),
    moonlightDiffuseColor(XMFLOAT4()),
    moonlightSpecularColor(XMFLOAT4()),
    moonlightSpecularIntensity(0.0f),
    launchPadScale(XMFLOAT3()),
    launchPadTessellationSettings(XMFLOAT4()),
    launchPadDisplacementSettings(XMFLOAT4()),
    antTweakBarConfigData(nullptr)
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

        if (0 == strcmp(cmd, "CameraInitialPosition"))
        {
            fin >> x >> y >> z;
             cameraPosition = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "TerrainSize"))
        {
            fin >> x >> y >> z;
             terrainSize = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "TerrainInitialScale"))
        {
            fin >> x >> y >> z;
             terrainScale = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "RocketInitialPosition"))
        {
            fin >> x >> y >> z;
             rocketPosition = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "RocketInitialRotation"))
        {
            fin >> x >> y >> z;
             rocketRotation = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "RocketInitialScale"))
        {
            fin >> x >> y >> z;
             rocketScale = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "SunlightAmbientColor"))
        {
            fin >> x >> y >> z >> w;
             sunlightAmbientColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "SunlightDiffuseColor"))
        {
            fin >> x >> y >> z >> w;
             sunlightDiffuseColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "SunlightSpecularColor"))
        {
            fin >> x >> y >> z >> w;
             sunlightSpecularColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "SunlightSpecularIntensity"))
        {
            fin >> x;
             sunlightSpecularIntensity = x;
        }
        else if (0 == strcmp(cmd, "MoonlightAmbientColor"))
        {
            fin >> x >> y >> z >> w;
             moonlightAmbientColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "MoonlightDiffuseColor"))
        {
            fin >> x >> y >> z >> w;
             moonlightDiffuseColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "MoonlightSpecularColor"))
        {
            fin >> x >> y >> z >> w;
             moonlightSpecularColor = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "MoonlightSpecularIntensity"))
        {
            fin >> x;
             moonlightSpecularIntensity = x;
        }
        else if (0 == strcmp(cmd, "LaunchPadInitialScale"))
        {
            fin >> x >> y >> z;
             launchPadScale = XMFLOAT3(x, y, z);
        }
        else if (0 == strcmp(cmd, "LaunchPadTessellationSettings"))
        {
            fin >> x >> y >> z >> w;
             launchPadTessellationSettings = XMFLOAT4(x, y, z, w);
        }
        else if (0 == strcmp(cmd, "LaunchPadDisplacementSettings"))
        {
            fin >> x >> y >> z >> w;
             launchPadDisplacementSettings = XMFLOAT4(x, y, z, w);
        }
    }


	 antTweakBarConfigData = TwNewBar("Configuration");
	TwDefine(" Configuration label='Configuration File Data' position='1120 20' size='220 320'");
	TwAddVarRW( antTweakBarConfigData, "CameraPosition: ", TW_TYPE_DIR3F, &cameraPosition, "");
	TwAddVarRW( antTweakBarConfigData, "TerrainDim: ", TW_TYPE_DIR3F, &terrainSize, "");
	TwAddVarRW( antTweakBarConfigData, "TerrainScale: ", TW_TYPE_DIR3F, &terrainScale, "");
	TwAddVarRW( antTweakBarConfigData, "RocketPosition: ", TW_TYPE_DIR3F, &rocketPosition, "");
	TwAddVarRW( antTweakBarConfigData, "RocketRotation: ", TW_TYPE_DIR3F, &rocketRotation, "");
	TwAddVarRW( antTweakBarConfigData, "RocketScale: ", TW_TYPE_DIR3F, &rocketScale, "");

	TwAddVarRW( antTweakBarConfigData, "SunAmbient: ", TW_TYPE_COLOR4F, &sunlightAmbientColor, "");
	TwAddVarRW( antTweakBarConfigData, "SunDiffuse: ", TW_TYPE_COLOR4F, &sunlightDiffuseColor, "");
	TwAddVarRW( antTweakBarConfigData, "SunSpecular: ", TW_TYPE_COLOR4F, &sunlightSpecularColor, "");
	TwAddVarRW( antTweakBarConfigData, "SunSpecularPower: ", TW_TYPE_FLOAT, &sunlightSpecularIntensity, "");

	TwAddVarRW( antTweakBarConfigData, "MoonAmbient: ", TW_TYPE_COLOR4F, &moonlightAmbientColor, "");
	TwAddVarRW( antTweakBarConfigData, "MoonDiffuse: ", TW_TYPE_COLOR4F, &moonlightDiffuseColor, "");
	TwAddVarRW( antTweakBarConfigData, "MoonSpecular: ", TW_TYPE_COLOR4F, &moonlightSpecularColor, "");
	TwAddVarRW( antTweakBarConfigData, "MoonSpecularPower: ", TW_TYPE_FLOAT, &moonlightSpecularIntensity, "");
	TwAddVarRW( antTweakBarConfigData, "LaunchPadScale: ", TW_TYPE_DIR3F, & launchPadScale, "");
	TwAddVarRW( antTweakBarConfigData, "LaunchPadTess: ", TW_TYPE_QUAT4F, & launchPadTessellationSettings, "");
	TwAddVarRW( antTweakBarConfigData, "LaunchPadDisp: ", TW_TYPE_QUAT4F, & launchPadDisplacementSettings, "");
}

SimulationConfigLoader::~SimulationConfigLoader()
{
	TwTerminate();
}

const XMFLOAT3& SimulationConfigLoader::GetCameraPosition() const
{
	return  cameraPosition;
}

const XMFLOAT3& SimulationConfigLoader::GetTerrainDimensions() const
{
	return  terrainSize;
}

const XMFLOAT3& SimulationConfigLoader::GetTerrainScale() const
{
	return  terrainScale;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketPosition() const
{
	return  rocketPosition;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketRotation() const
{
	return  rocketRotation;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketScale() const
{
	return  rocketScale;
}

const XMFLOAT4& SimulationConfigLoader::GetSunAmbient() const
{
	return  sunlightAmbientColor;
}

const XMFLOAT4& SimulationConfigLoader::GetSunDiffuse() const
{
	return  sunlightDiffuseColor;
}

const XMFLOAT4& SimulationConfigLoader::GetSunSpecular() const
{
	return  sunlightSpecularColor;
}

const float SimulationConfigLoader::GetSunSpecularPower() const
{
	return  sunlightSpecularIntensity;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonAmbient() const
{
	return  moonlightAmbientColor;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonDiffuse() const
{
	return  moonlightDiffuseColor;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonSpecular() const
{
	return  moonlightSpecularColor;
}

const float SimulationConfigLoader::GetMoonSpecularPower() const
{
	return  moonlightSpecularIntensity;
}

const XMFLOAT3& SimulationConfigLoader::GetLaunchPadScale() const
{
	return  launchPadScale;
}

const XMFLOAT4& SimulationConfigLoader::GetLaunchPadTessellationValues() const
{
	return  launchPadTessellationSettings;
}

const XMFLOAT4& SimulationConfigLoader::GetLaunchPadDisplacementValues() const
{
	return  launchPadDisplacementSettings;
}







