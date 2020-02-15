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

$e = new \VPack\Exception('Test error message');

$sd = $m->serialize($e); // {"message":"Example error message"} in VPack with tag=5

var_dump($sd);

$dd = $m->deserialize($sd); // \VPack\Exception object

var_dump($dd);
