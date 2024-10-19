<?php

/**
 * The Date module provides three different date formats
 *  'datetime'  Date
 *  'date'      Date (ISO format)
 *  'datestamp' Date (Unix timestamp)
 */

// Functionality depends on Date
if (module_exists('date')) {

  /**
   * Implementation of hook_user_import_form_field_match().
   *
   * Add supported date fields to the dropdown list.
   * Commented out for now, as this is done by hacking field.inc:
   * https://drupal.stackexchange.com/q/281761/12076
   */
  /*
  function date_user_import_form_field_match() {
  return $options;
  }
   */

  /**
   * Implementation of hook_user_import_data().
   */
  function date_user_import_data($settings, $update_setting, $column_settings, $module, $field_id, $data, $column_id) {
    if ($module != 'date') {
      return;
    }
    $value = $data[$column_id];
    return $value;
  }

  /**
   * Implementation of hook_user_import_after_save().
   */
  function date_user_import_after_save($settings, $account, $password, $fields, $updated, $update_setting_per_module) {

    $fields_data = isset($fields['field']) ? $fields['field'] : array();

    if (empty($fields_data)) {
      return;
    }

    $processors       = field_user_import_supported_fields();
    $delimiter        = $settings['multi_value_delimiter'];
    $user_fields      = user_load($account->uid);
    $processed_fields = array();

    foreach ($fields_data as $field_name => $data) {
      $field_info         = field_info_field($field_name);
      $field_type         = $field_info['type'];
      $processor_function = $processors[$field_type]['save'];

      // Deal with multiple values.
      $values = explode($delimiter, $data[0]);

      // Use callback function to process info for field before saving.
      $processed_fields[$field_name] = $processor_function($user_fields, $field_name, $values);
    }

    if (!empty($processed_fields)) {
      //user_save($account, $processed_fields);
    }

    return;
  }

  /**
   * TODO: Support there being an end date.
   *
   * Note that strtotime() has a range limit between UTC
   * 1901-12-13T20:45:54 and 2038-01-19T03:14:07.
   */
  function date_user_import_field_processor($user_fields, $field_name, $values) {

    $field = $user_fields->$field_name;

    for ($i = 0; $i < count($values); $i++) {
      if (!empty($values[$i])) {
        $field[LANGUAGE_NONE][$i]['value'] = $values[$i];
      }
    }

    // Make sure it is IDO 8601 format YYYY-MM-DDTHH:MM:SS
    $date = $field[LANGUAGE_NONE][0]['value'];
    $datetime = date('Y-m-d\TH:i:s', strtotime($date));
    $field[LANGUAGE_NONE][0]['value'] = $datetime;
    return $field;
  }

}
