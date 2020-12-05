--TEST--
Test TypeMapper with Serializable
--FILE--
<?php
class ExampleError implements \VPack\Serializable
{
	protected $message;

	public function __construct($message)
	{
		$this->message = $message;
	}

	public function getMessage()
	{
		return $this->message;
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

$input = new ExampleError("Example error message");

$r = $m->serialize($input);

echo implode(" ", str_split(bin2hex($r), 2))."\n";

$d = $m->deserialize($r);

var_dump($input->getMessage() === $d->getMessage());
var_dump(get_class($input) === get_class($d));

?>
--EXPECT--
ee 05 14 21 47 6d 65 73 73 61 67 65 55 45 78 61 6d 70 6c 65 20 65 72 72 6f 72 20 6d 65 73 73 61 67 65 01
bool(true)
bool(true)