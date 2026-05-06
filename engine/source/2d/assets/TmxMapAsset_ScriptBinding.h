
ConsoleMethod(TmxMapAsset, setMapFile, void, 3, 3,     "(MapFile) Sets the map file (tmx file).\n"
	"@return No return value.")
{
	object->setMapFile( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(TmxMapAsset, getMapFile, const char*, 2, 2,  "() Gets the map file.\n"
	"@return Returns the tmx map file.")
{
	return object->getMapFile();
}

ConsoleMethod(TmxMapAsset, getOrientation, const char*, 2, 2,  "() Gets the map orientation.\n"
	"@return Returns the tmx map orientation layout.")
{
	return object->getOrientation();
}

ConsoleMethod(TmxMapAsset, getLayerCount, S32, 2, 2,  "() Gets the number of tile layers.\n"
	"@return Returns the numer of tile layers in the map.")
{
	return object->getLayerCount();
}