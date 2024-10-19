<?php
/**
 * @file
 * ArrayDataItemIterator class file.
 */

/**
 * Class ArrayDataItemIterator
 */
class ArrayDataItemIterator extends ArrayIterator implements RecursiveIterator {

  /**
   * Get the children of the current element.
   *
   * @return ArrayDataItemIterator
   */
  public function getChildren(): RecursiveIterator {
    $item = $this->current();

    if (!is_array($item) || !isset($item['children'])) {
      throw new UnexpectedValueException("Current item must be an array with a 'children' key");
    }

    return new ArrayDataItemIterator($item['children']);
  }

  /**
   * Check if the current element has children.
   *
   * @return bool
   */
  public function hasChildren(): bool {
    $item = $this->current();

    return is_array($item) && !empty($item['children']);
  }
}
