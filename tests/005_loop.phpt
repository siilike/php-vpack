--TEST--
Run TypeMapper in a loop
--FILE--
<?php
$json = <<<EOF
[{"_id":"5fcbed323df87727d5998b43","index":0,"guid":"85a31145-2439-45dc-ac9e-311838e9ab4a","isActive":false,"balance":"$2,684.48","picture":"http://placehold.it/32x32","age":24,"eyeColor":"blue","name":{"first":"Renee","last":"Gentry"},"company":"XANIDE","email":"renee.gentry@xanide.org","phone":"+1 (878) 530-3300","address":"270 Osborn Street, Yorklyn, Montana, 3084","about":"Non id tempor aliqua nulla reprehenderit ullamco exercitation minim esse minim ipsum. Mollit officia occaecat excepteur sint. Sint consectetur in labore nisi ut aute labore eu laborum consequat sint excepteur eiusmod. Est sint proident ea minim anim nostrud sunt ad enim ullamco quis. Laboris anim ad ex qui do adipisicing.","registered":"Wednesday, April 23, 2014 10:28 AM","latitude":"-4.280711","longitude":"179.025284","tags":["enim","officia","ut","aliquip","dolore"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Ophelia Blackburn"},{"id":1,"name":"Shelton Bean"},{"id":2,"name":"Shepherd Stevens"}],"greeting":"Hello, Renee! You have 8 unread messages.","favoriteFruit":"banana"},{"_id":"5fcbed327074764b66c510a3","index":1,"guid":"1732bd47-93d2-415a-a868-0c50300385dd","isActive":false,"balance":"$3,742.85","picture":"http://placehold.it/32x32","age":22,"eyeColor":"blue","name":{"first":"Julianne","last":"Hurley"},"company":"SUPPORTAL","email":"julianne.hurley@supportal.name","phone":"+1 (994) 492-2128","address":"375 Merit Court, Keyport, Indiana, 8055","about":"Amet enim tempor magna esse reprehenderit commodo tempor. Irure minim ipsum nisi excepteur ex id fugiat. Sint consectetur elit ullamco ullamco dolor. Ut pariatur eiusmod ea minim culpa ea sit anim reprehenderit. Pariatur pariatur ut nisi esse.","registered":"Monday, September 18, 2017 7:50 PM","latitude":"-10.650159","longitude":"-144.72196","tags":["officia","elit","dolor","deserunt","magna"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Mabel Hansen"},{"id":1,"name":"Lenore Bolton"},{"id":2,"name":"Lopez Galloway"}],"greeting":"Hello, Julianne! You have 6 unread messages.","favoriteFruit":"apple"},{"_id":"5fcbed32d0db1027ea35e885","index":2,"guid":"66d1d498-0332-4ec6-9049-21b56bcc89d1","isActive":false,"balance":"$1,024.67","picture":"http://placehold.it/32x32","age":20,"eyeColor":"blue","name":{"first":"Noemi","last":"Dale"},"company":"QUALITERN","email":"noemi.dale@qualitern.us","phone":"+1 (902) 516-2862","address":"988 Rugby Road, Cannondale, Missouri, 3604","about":"Velit excepteur do amet anim sit cillum sint. Cillum deserunt qui occaecat sunt nisi cillum in pariatur ad. Pariatur adipisicing incididunt esse amet sint incididunt elit ut ad ipsum quis. Pariatur consequat minim voluptate eiusmod ad pariatur veniam id est. Reprehenderit esse in nulla nulla nisi aliqua enim officia veniam qui cupidatat minim magna nostrud.","registered":"Tuesday, September 22, 2020 4:30 PM","latitude":"-39.91155","longitude":"-19.765503","tags":["minim","magna","aliquip","deserunt","irure"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Charles Baldwin"},{"id":1,"name":"Monroe Spence"},{"id":2,"name":"Cantu Gates"}],"greeting":"Hello, Noemi! You have 8 unread messages.","favoriteFruit":"banana"},{"_id":"5fcbed323aba1bc99cf91d09","index":3,"guid":"fc41610c-a8c6-4302-8087-6540d35f3c67","isActive":false,"balance":"$2,085.82","picture":"http://placehold.it/32x32","age":39,"eyeColor":"green","name":{"first":"Francesca","last":"Cohen"},"company":"ISOSPHERE","email":"francesca.cohen@isosphere.ca","phone":"+1 (846) 548-2453","address":"975 Etna Street, Alderpoint, Wisconsin, 6495","about":"Sint aliqua adipisicing anim minim sunt irure tempor tempor laboris. Do elit consectetur non ea esse mollit. Labore et occaecat ullamco ipsum nostrud eu minim anim minim. Incididunt nulla excepteur labore ex duis irure aute culpa excepteur velit sit nostrud excepteur ullamco. Officia incididunt magna dolore veniam adipisicing anim velit non in irure id nostrud dolor. Ullamco consequat cupidatat quis ipsum enim eiusmod eiusmod elit id eu deserunt anim eiusmod. Aliqua anim duis pariatur eu eu consectetur anim et.","registered":"Monday, April 6, 2015 6:30 PM","latitude":"-28.443893","longitude":"149.862517","tags":["labore","duis","dolor","non","amet"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Denise Hines"},{"id":1,"name":"Strickland Delgado"},{"id":2,"name":"Head Keith"}],"greeting":"Hello, Francesca! You have 8 unread messages.","favoriteFruit":"strawberry"},{"_id":"5fcbed327aa960ac8c4bc5d5","index":4,"guid":"b5475ba7-b7be-47a0-81b5-e0e47ab1a3b2","isActive":false,"balance":"$3,738.74","picture":"http://placehold.it/32x32","age":33,"eyeColor":"brown","name":{"first":"Katie","last":"Heath"},"company":"SNORUS","email":"katie.heath@snorus.co.uk","phone":"+1 (876) 571-3676","address":"782 Howard Alley, Greenfields, Oregon, 6064","about":"Fugiat fugiat occaecat labore enim eu proident veniam. Fugiat et ex aliquip ea eu nostrud quis ipsum aliqua. Aute excepteur sit proident commodo cupidatat laborum non voluptate cupidatat incididunt. Est excepteur Lorem eiusmod officia fugiat mollit ipsum quis dolore Lorem.","registered":"Saturday, February 10, 2018 12:17 PM","latitude":"56.63443","longitude":"-8.910974","tags":["veniam","nulla","duis","nisi","ex"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Maggie Weeks"},{"id":1,"name":"Foster Ratliff"},{"id":2,"name":"Leach Joseph"}],"greeting":"Hello, Katie! You have 5 unread messages.","favoriteFruit":"banana"},{"_id":"5fcbed32c01ae616d230eba9","index":5,"guid":"de422a13-7c37-49ad-8145-c022a92c2600","isActive":true,"balance":"$3,303.27","picture":"http://placehold.it/32x32","age":30,"eyeColor":"brown","name":{"first":"Gregory","last":"Fowler"},"company":"IMPERIUM","email":"gregory.fowler@imperium.net","phone":"+1 (851) 488-2121","address":"838 Bainbridge Street, Bellamy, Virgin Islands, 5636","about":"Adipisicing culpa sit nostrud pariatur ea incididunt ullamco nostrud magna. Voluptate esse cillum in nisi duis id qui. Id do amet non ad dolore cupidatat id. Dolore ut sunt est dolore irure magna velit ex et ad non non. Enim dolore ipsum ut do non sunt enim nulla ipsum labore aliqua nulla. Voluptate ullamco consequat eu elit quis reprehenderit nulla magna pariatur magna est id culpa.","registered":"Monday, April 27, 2015 1:40 PM","latitude":"-12.01294","longitude":"-80.576908","tags":["sunt","veniam","enim","aliquip","qui"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Bernice Rodriquez"},{"id":1,"name":"Martin Solomon"},{"id":2,"name":"Nettie Grant"}],"greeting":"Hello, Gregory! You have 10 unread messages.","favoriteFruit":"apple"},{"_id":"5fcbed328e39af13bb147705","index":6,"guid":"50579897-77b7-4724-ab6a-b54e3e7d7a27","isActive":true,"balance":"$2,324.16","picture":"http://placehold.it/32x32","age":30,"eyeColor":"brown","name":{"first":"Sheppard","last":"Talley"},"company":"MANGLO","email":"sheppard.talley@manglo.com","phone":"+1 (949) 465-2114","address":"411 Dank Court, Delco, Colorado, 8968","about":"Aute tempor pariatur non est sunt adipisicing mollit esse Lorem fugiat enim est cupidatat. Elit pariatur enim elit dolore excepteur Lorem elit proident culpa. Nulla aliqua adipisicing aliqua dolor exercitation non enim adipisicing consectetur veniam amet. Officia qui Lorem eiusmod excepteur reprehenderit in reprehenderit quis ipsum commodo ea. Proident nulla laboris velit excepteur aliquip excepteur nisi.","registered":"Monday, May 4, 2015 3:02 PM","latitude":"-77.107056","longitude":"-82.771281","tags":["aute","veniam","ex","consectetur","proident"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Millicent Zamora"},{"id":1,"name":"Beach Hammond"},{"id":2,"name":"Case Stark"}],"greeting":"Hello, Sheppard! You have 10 unread messages.","favoriteFruit":"strawberry"},{"_id":"5fcbed32bf8a87478a53d30d","index":7,"guid":"691c8e79-dc48-4355-8b47-2582f150de97","isActive":false,"balance":"$2,875.37","picture":"http://placehold.it/32x32","age":37,"eyeColor":"brown","name":{"first":"Hahn","last":"Schwartz"},"company":"OBLIQ","email":"hahn.schwartz@obliq.biz","phone":"+1 (958) 474-3572","address":"369 Dictum Court, Lynn, Georgia, 2955","about":"Eu fugiat anim occaecat in irure reprehenderit laborum ea. Voluptate dolore pariatur anim eiusmod eu voluptate veniam ad sit excepteur non in. Velit nisi officia laborum ea dolore minim id mollit. Excepteur veniam ipsum veniam cillum non consequat quis. Non non ea ut deserunt veniam id officia nulla quis cupidatat cillum nostrud nisi exercitation.","registered":"Friday, July 10, 2015 5:13 PM","latitude":"10.584548","longitude":"101.598492","tags":["eu","cupidatat","id","commodo","occaecat"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Alicia Yang"},{"id":1,"name":"Iris Guerrero"},{"id":2,"name":"Claudette Barnes"}],"greeting":"Hello, Hahn! You have 5 unread messages.","favoriteFruit":"banana"},{"_id":"5fcbed32499df7673b5c99f3","index":8,"guid":"bebc7568-8d00-4fb9-a110-e666d5631d2b","isActive":true,"balance":"$3,063.07","picture":"http://placehold.it/32x32","age":27,"eyeColor":"brown","name":{"first":"Nicholson","last":"Keller"},"company":"ANDERSHUN","email":"nicholson.keller@andershun.biz","phone":"+1 (847) 403-3512","address":"430 Cedar Street, Williston, Marshall Islands, 3506","about":"Reprehenderit cillum est nisi fugiat elit Lorem voluptate mollit incididunt. Officia eu fugiat labore velit excepteur et voluptate elit. Deserunt veniam veniam anim exercitation enim tempor ut quis quis excepteur. Et exercitation id in minim. Anim Lorem Lorem reprehenderit ex aliqua aliquip.","registered":"Friday, November 22, 2019 10:31 AM","latitude":"-41.801166","longitude":"-77.978336","tags":["do","eu","qui","reprehenderit","consequat"],"range":[0,1,2,3,4,5,6,7,8,9],"friends":[{"id":0,"name":"Wise Roth"},{"id":1,"name":"Pennington Smith"},{"id":2,"name":"Odonnell Callahan"}],"greeting":"Hello, Nicholson! You have 8 unread messages.","favoriteFruit":"banana"}]
EOF;

$jsonDecoded = json_decode($json, true);

$m = new \VPack\TypeMapper();

function array_diff_assoc_recursive($array1, $array2) {
	$difference = array();
	foreach($array1 as $key => $value) {
		if(is_array($value)) {
			if(!isset($array2[$key]) || !is_array($array2[$key])) {
				$difference[$key] = $value;
			} else {
				$new_diff = array_diff_assoc_recursive($value, $array2[$key]);
				if(!empty($new_diff))
					$difference[$key] = $new_diff;
			}
		} else if( !array_key_exists($key,$array2) || $array2[$key] !== $value ) {
			$difference[$key] = $value;
		}
	}
	return $difference;
}

$input = $jsonDecoded;
for($i = 0; $i < 10000; $i++)
{
	$r = $m->serialize($input) . " 123 ";
	$d = $m->deserialize($r);
	$input = $d;
}

var_dump(array_diff_assoc_recursive($jsonDecoded, $input));
?>
--EXPECT--
array(0) {
}