<?hh

function dumpcourse($current) :mixed{
	$title = ($current->nodeType != XML_TEXT_NODE && $current->hasAttribute('title')) ? $current->getAttribute('title'):"no title";
	echo "Course: $title:";echo(get_class($current)), "\n";
	echo "~";var_dump($current->textContent);
}

<<__EntryPoint>>
function main() :mixed{
  $xml = <<< EOXML
<?xml version="1.0" encoding="ISO-8859-1"?>
<courses>
  <course title="one">
    <notes>
      <note>c1n1</note>
      <note>c1n2</note>
    </notes>
  </course>
  <course title="two">
    <notes>
      <note>c2n1</note>
      <note>c2n2</note>
    </notes>
  </course>
</courses>
EOXML;

  $dom = new DOMDocument();
  $dom->loadXML($xml);
  $root = $dom->documentElement;

  // strip all text nodes from this tree
  $children = $root->childNodes;
  $len = $children->length;
  for ($index = $children->length - 1; $index >=0; $index--) {
    $current = $children->item($index);
    if ($current->nodeType == XML_TEXT_NODE) {
      $noderemoved = $root->removeChild($current);
    }
  }

  echo "Start cloneNode test\n";
  $first_course = $children->item(0);
  $cloned_first_course_default = $first_course->cloneNode();
  $first_course->setAttribute('title', 'new title1');

  $cloned_first_course_true = $first_course->cloneNode(true);
  $first_course->setAttribute('title', 'new title2');

  $cloned_first_course_false = $first_course->cloneNode(false);
  $first_course->setAttribute('title', 'new title3');

  $cloned_first_course_default->setAttribute('title', 'new title default');
  $cloned_first_course_true->setAttribute('title', 'new title true');
  $cloned_first_course_false->setAttribute('title', 'new title false');

  $root->appendChild($cloned_first_course_default);
  $root->appendChild($cloned_first_course_true);
  $root->appendChild($cloned_first_course_false);

  $children = $root->childNodes;
  for ($index = 0; $index < $children->length; $index++) {
    echo "node $index\n";
    dumpcourse($children->item($index));
  }
}

