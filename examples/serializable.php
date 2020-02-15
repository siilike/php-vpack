<?php
class ExampleError implements \VPack\Serializable
{
	protected $message;

	public function __construct($message)
	{
		$this->message = $message;
	}

	public function toVPack($b, $m)
	{
		$b->openObject();
		$b->addObjectValue('message', 0, $this->message);
		$b->close();
	}

	public static function fromVPack($a, $m)
	{
		return new ExampleError($a->get('message')->getString());
	}

	public function getTagId()
	{
		return 5;
	}
}

$m = new \VPack\TypeMapper();
$m->registerMapping(5, 'ExampleError');

$e = new ExampleError("Example error message");

$sd = $m->serialize($e); // {"message":"Example error message"} in VPack with tag=5

var_dump($sd);

$dd = $m->deserialize($sd); // ExampleError object

var_dump($dd);
