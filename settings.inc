<?php
// $Id$
function om_flowsheet_settings_form($form, $form_state)
{
	$form['emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Bcc) Notification emails'),
		'#description' => t('Specify emails id for Bcc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('om_flowsheet_emails', '')
	);
	$form['cc_emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Cc) Notification emails'),
		'#description' => t('Specify emails id for Cc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('om_flowsheet_cc_emails', '')
	);
	$form['from_email'] = array(
		'#type' => 'textfield',
		'#title' => t('Outgoing from email address'),
		'#description' => t('Email address to be display in the from field of all outgoing messages'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('om_flowsheet_from_email', '')
	);
	$form['extensions']['abstract_upload'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed abstract file extensions'),
		'#description' => t('A comma separated list WITHOUT SPACE of pdf file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('om_flowsheet_abstract_upload_extensions', '')
	);
	$form['extensions']['flowsheet_upload'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed om flowsheet for the developed process'),
		'#description' => t('A comma separated list WITHOUT SPACE of pdf file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('om_flowsheet_flowsheet_developed_process_source_extensions', '')
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
function om_flowsheet_settings_form_validate($form, &$form_state)
{
	return;
}
function om_flowsheet_settings_form_submit($form, &$form_state)
{
	variable_set('om_flowsheet_emails', $form_state['values']['emails']);
	variable_set('om_flowsheet_cc_emails', $form_state['values']['cc_emails']);
	variable_set('om_flowsheet_from_email', $form_state['values']['from_email']);
	variable_set('om_flowsheet_abstract_upload_extensions', $form_state['values']['abstract_upload']);
	variable_set('om_flowsheet_flowsheet_developed_process_source_extensions', $form_state['values']['flowsheet_upload']);
	drupal_set_message(t('Settings updated'), 'status');
}
