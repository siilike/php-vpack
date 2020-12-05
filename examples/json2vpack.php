<?php
if(!empty($argv[1]))
{
	$input = json_decode(file_get_contents($argv[1]), true);
}
else
{
	$input = json_decode(stream_get_contents(fopen('php://stdin', 'r')), true);
}

if($input === null)
{
	echo "Invalid input\n";
	exit(1);
}

echo (new \VPack\TypeMapper())->serialize($input);
