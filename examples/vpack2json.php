<?php
if(!empty($argv[1]))
{
	$input = file_get_contents($argv[1]);
}
else
{
	$input = stream_get_contents(fopen('php://stdin', 'r'));
}

echo json_encode((new \VPack\TypeMapper())->deserialize($input), JSON_PRETTY_PRINT);
