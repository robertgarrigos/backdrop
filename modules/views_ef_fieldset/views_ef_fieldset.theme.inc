<?php
/**
 * @file
 * Module file containing theme implementations.
 */

/**
 * Theme callback to render the option form as a draggable table.
 */
function theme_views_ef_fieldset_reorder_form($vars) {
  $form = $vars['form'];
  $table_rows = array();

  foreach (element_children($form) as $key) {
    $row = array();
    $classes = array('draggable');
    $element = $form[$key];
    $field = $element['weight']['#item'];

    $indent = theme('indentation', array('size' => $field['depth']));

    $title = $field['title'];
    if ($field['type'] == 'container') {
      if ($field['pid'] == '') {
        $title = '<em>' . $title . '</em>';
        $classes = array('tabledrag-root');
      }
      $title = '<strong>' . $title . '</strong>';
    }

    $row[] = $indent . $title . backdrop_render($element['id']) . backdrop_render($element['pid']);
    $row[] = backdrop_render($element['container_type']);
    $row[] = backdrop_render($element['title']);
    $row[] = backdrop_render($element['description']);
    $row[] = backdrop_render($element['collapsible']);
    $row[] = backdrop_render($element['collapsed']);
    $row[] = backdrop_render($element['weight']);

    $table_rows[] = array(
      'data' => $row,
      'class' => $classes,
      'id' => 'display-row-' . $key,
    );
  }

  backdrop_add_tabledrag('reorder-fieldset-filters', 'order', 'sibling', 'item-weight');
  backdrop_add_tabledrag('reorder-fieldset-filters', 'order', 'sibling', 'item-weight', 'subitem-weight');
  backdrop_add_tabledrag('reorder-fieldset-filters', 'match', 'parent', 'item-pid', 'item-pid', 'item-id', FALSE);

  $header = array(t('Item'), t('Type'), t('Title'), t('Description'), t('Collapsible'), t('Collapsed'), '');

  $output = '';
  $output .= theme('table',
    array(
    'header' => $header,
    'rows' => $table_rows,
    'attributes' => array('id' => 'reorder-fieldset-filters'),
  ));

  return $output;
}
