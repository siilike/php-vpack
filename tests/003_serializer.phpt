--TEST--
Test TypeMapper with Serializer
--FILE--
<?php
class ExampleSerializer implements \VPack\Serializer
{
	public function serialize($a, $b, $m)
	{
		$b->openObject();
		$b->addObjectValue('message', 0, $a->getMessage());
		$b->close();
	}

	public function deserialize($a, $m)
	{
		return new \VPack\Exception($a->get('message')->getString());
	}

	public function getTagId()
	{
		return 5;
	}
}

$serializer = new ExampleSerializer();

$m = new \VPack\TypeMapper();
$m->registerMapping($serializer->getTagId(), 'VPack\Exception');

$m->registerSerializer($serializer->getTagId(), array(&$serializer, 'serialize'));
$m->registerDeserializer($serializer->getTagId(), array(&$serializer, 'deserialize'));

$input = new \VPack\Exception('Test error message');

$r = $m->serialize($input);

echo implode(" ", str_split(bin2hex($r), 2))."\n";

$d = $m->deserialize($r);

var_dump($input->getMessage() === $d->getMessage());
var_dump(get_class($input) === get_class($d));
?>
--EXPECT--
ee 05 14 1e 47 6d 65 73 73 61 67 65 52 54 65 73 74 20 65 72 72 6f 72 20 6d 65 73 73 61 67 65 01
bool(true)
bool(true)