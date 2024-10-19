<?php

/**
 * @file
 * {@inheritdoc}
 */

define('ACCOUNT_ACTIVATION_REMINDER_BODY',
'Hello [user:name],

This is a reminder that an account has been created for you at [site:name].

To enjoy the full benefits that you have signed up for please complete the
account verification process by following the link below:

[user:one-time-login-url]

This link can only be used once to log in and will lead you to a page where you can set your password.

After setting your password, you will be able to log in at [site:login-url] in the future using:

username: [user:name]
password: Your password

We look forward to having you be a part of our site.

Sincerely,
[site:name]'
);

/**
 * Form builder.
 *
 * Create and display the Account Activation Reminder settings form.
 */
function account_activation_reminder_form($form, &$form_state) {
  $config = config('account_activation_reminder.settings');
  // Text field for the e-mail subject.
  $form['#config'] = 'account_activation_reminder.settings';
  $form['account_activation_reminder_e-mail_subject'] = array(
    '#type' => 'textfield',
    '#default_value' => $config->get('account_activation_reminder_e-mail_subject'),
    '#title' => t('Account Activation Reminder e-mail subject'),
    '#description' => t('The subject of the e-mail which will be sent to users'),
    '#size' => 40,
    '#maxlength' => 120,
    '#required' => TRUE,
  );

  // Text area for the body of the e-mail.
  $form['account_activation_reminder_e-mail_body'] = array(
    '#type' => 'textarea',
    '#rows' => 10,
    '#columns' => 40,
    '#default_value' => $config->get('account_activation_reminder_e-mail_body'),
    '#title' => t('Account Activation e-mail text'),
    '#required' => TRUE,
    '#description' => t('The body of the e-mail which will be sent to the user. Be sure to include the token [user:one-time-login-url] otherwise the email will not contain the one-time login link'),
  );

  return system_settings_form($form);
}

/**
 * Form builder; display the e-mail confirmation form.
 */
function account_activation_reminder_confirm_form($form, &$form_state, $uid) {
  $form['account'] = array(
    '#type' => 'value',
    '#value' => user_load($uid),
  );

  return confirm_form(
        $form,
        t('Are you sure you want to resend an activation email to this user?'),
        'user/' . $uid,
        t('This action can not be undone.'),
        t('Send e-mail'),
        t('Cancel')
    );
}
