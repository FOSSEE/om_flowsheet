<?php
// $Id$
/*
Approval Status :
0 - Pending
1 - Approved
2 - Dis-Approved
3 - Completed
Solution Status :
0 - Pending
1 - Approved
2 - Dis-Approved
Solution Display :
0 - No
1 - Yes


is_completed
0 - in progress
1 - Completed
Tables :
om_flowsheet_solution : approval_status
0 - Pending
1 - Approved
2 - Disapproved (delete it)
*/
function om_flowsheet_proposal_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	/************************ start approve book details ************************/
	if ($user->uid == 0)
	{
		$msg = drupal_set_message(t('It is mandatory to ' . l('login', 'user') . ' on this website to access the flowsheet proposal form. If you are new user please create a new account first.'), 'error');
		//drupal_goto('om-flowsheet-project');
		drupal_goto('user');
		return $msg;
	} //$user->uid == 0
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$proposal_q = $query->execute();
	$proposal_data = $proposal_q->fetchObject();
	if ($proposal_data)
	{
		if ($proposal_data->approval_status == 0 || $proposal_data->approval_status == 1)
		{
			drupal_set_message(t('We have already received your proposal.'), 'status');
			drupal_goto('chemical/flowsheeting-project');
			return;
		} //$proposal_data->approval_status == 0 || $proposal_data->approval_status == 1
	} //$proposal_data
	$imp = t('<span style="color: red;">*This is a mandatory field</span>');
	$form['#attributes'] = array(
		'enctype' => "multipart/form-data"
	);
	$form['name_title'] = array(
		'#type' => 'select',
		'#title' => t('Title'),
		'#options' => array(
			'Dr' => 'Dr',
			'Prof' => 'Prof',
			'Mr' => 'Mr',
			'Mrs' => 'Mrs',
			'Ms' => 'Ms'
		),
		'#required' => TRUE
	);
	$form['contributor_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of the contributor'),
		'#size' => 250,
		'#attributes' => array(
			'placeholder' => t('Enter your full name.....')
		),
		'#maxlength' => 250,
		'#required' => TRUE
	);
	$form['contributor_contact_no'] = array(
		'#type' => 'textfield',
		'#title' => t('Contact No.'),
		'#size' => 10,
		'#attributes' => array(
			'placeholder' => t('Enter your contact number')
		),
		'#maxlength' => 250
	);
	$form['month_year_of_degree'] = array(
		'#type' => 'date_popup',
		'#title' => t('Month and year of award of degree'),
		'#date_label_position' => '',
		'#description' => '',
		'#default_value' => '',
		'#date_format' => 'M-Y',
		'#date_increment' => 0,
		'#date_year_range' => '1960: +22',
		
		'#required' => TRUE
	);
	$form['contributor_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/ Institute'),
		'#size' => 80,
		'#maxlength' => 200,
		'#required' => TRUE,
		'#attributes' => array(
			'placeholder' => 'Insert full name of your institute/ university.... '
		)
	);
	$form['project_guide_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Project guide'),
		'#size' => 250,
		'#attributes' => array(
			'placeholder' => t('Enter full name of project guide')
		),
		'#maxlength' => 250
	);
	$form['project_guide_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Project guide email'),
		'#size' => 30
	);
	$form['project_guide_university'] = array(
		'#type' => 'textfield',
		'#title' => t('Project Guide University/ Institute'),
		'#size' => 80,
		'#maxlength' => 200,
		'#attributes' => array(
			'placeholder' => 'Insert full name of the institute/ university of your project guide.... '
		)
	);
	$form['country'] = array(
		'#type' => 'select',
		'#title' => t('Country'),
		'#options' => array(
			'India' => 'India',
			'Others' => 'Others'
		),
		'#required' => TRUE,
		'#tree' => TRUE,
		'#validated' => TRUE
	);
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your country name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#description' => $imp,
	);
	$form['other_state'] = array(
		'#type' => 'textfield',
		'#title' => t('State other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your state/region name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#description' => $imp,
	);
	$form['other_city'] = array(
		'#type' => 'textfield',
		'#title' => t('City other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your city name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#description' => $imp,
	);
	$form['all_state'] = array(
		'#type' => 'select',
		'#title' => t('State'),
		'#options' => _df_list_of_states(),
		'#validated' => TRUE,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		),
		'#description' => $imp
	);
	$form['city'] = array(
            '#type' => 'select',
            '#title' => t('City'),
            '#options' => _df_list_of_cities(),
            '#validated' => TRUE,
            '#states' => array(
                'visible' => array(
                    ':input[name="country"]' => array(
                        'value' => 'India'
                    )
                )
            ),
            '#description' => $imp,
	);
	$form['pincode'] = array(
		'#type' => 'textfield',
		'#title' => t('Pincode'),
		'#size' => 30,
		'#maxlength' => 6,
		'#required' => False,
		'#attributes' => array(
			'placeholder' => 'Enter pincode....'
		),
		'#required' => TRUE
	);
	/***************************************************************************/
	$form['hr'] = array(
		'#type' => 'item',
		'#markup' => '<hr>'
	);
	$form['dwsim_flowsheet_check'] = array(
            '#type' => 'radios',
            '#title' => t('Is the proposed OpenModelica flowsheet is from list of Completed DWSIM Flowsheets?'),
            '#options' => array(
			'1' => 'Yes',
			'0' => 'No',
			),
            '#validated' => TRUE,
	);
	$form['dwsim_flowsheet_name_dropdown'] = array(
            '#type' => 'select',
            '#title' => t('Select the name of flowsheet proposed.'),
            '#required' => TRUE,
            '#options' => _df_list_of_dwsim_flowsheets(),
           // '#description' => t('For example, if you are proposing flowsheet no. 10 from the list of completed DWSIM flowsheet, enter as: "10. Removal of Isopentane in Gasoline Manufacturing Plant"'),
            '#validated' => TRUE,
            '#states' => array(
                'visible' => array(
                    ':input[name="dwsim_flowsheet_check"]' => array(
                        'value' => '1'
                    )
                )
            ),
	);
	$form['project_title'] = array(
		'#type' => 'textarea',
		'#title' => t('Project Title'),
		'#size' => 250,
		'#description' => t('Maximum character limit is 250'),
		'#required' => TRUE,
		'#validated' => TRUE,
		'#states' => array(
                'visible' => array(
                    ':input[name="dwsim_flowsheet_check"]' => array(
                        'value' => '0'
                    )
                )
            ),
	);
	/*$form['dwsim_flowsheet_name'] = array(
            '#type' => 'textfield',
            '#title' => t('Enter the exact name of flowsheet along with its serial number.'),
            '#required' => TRUE,
            '#description' => t('For example, if you are proposing flowsheet no. 10 from the list of completed DWSIM flowsheet, enter as: "10. Removal of Isopentane in Gasoline Manufacturing Plant"'),
            '#validated' => TRUE,
            '#states' => array(
                'visible' => array(
                    ':input[name="dwsim_flowsheet_check"]' => array(
                        'value' => '1'
                    )
                )
            ),
	);*/ 
	$form['reference'] = array(
		'#type' => 'textfield',
		'#title' => t('Reference'),
		'#size' => 250,
		'#maxlength' => 250,
		'#required' => TRUE,
		'#attributes' => array(
			'placeholder' => 'Enter reference'
		)
	);
	$form['version'] = array(
		'#type' => 'select',
		'#title' => t('Version'),
		'#options' => _om_df_list_of_software_version(),
		'#required' => TRUE
	);
	$form['older'] = array(
		'#type' => 'textfield',
		'#title' => t('Other Version'),
		'#size' => 30,
		'#maxlength' => 50,
		//'#required' => TRUE,
		'#description' => t('Specify the Older version used as format "om v2.0"'),
		'#states' => array(
			'visible' => array(
				':input[name="version"]' => array(
					'value' => 'Old version'
				)
			)
		)
	);
	$form['process_development_compound_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of compound for which process development is carried out'),
		'#size' => 50,
		'#description' => t('Mention the compound name as shown:
Ex: Ethanol'),
		'#required' => TRUE
	);
	$form['process_development_compound_cas_no'] = array(
		'#type' => 'textfield',
		'#title' => t('CAS number for compound which process development is carried out'),
		'#size' => 50,
		'#description' => t('Mention the compound CAS No as shown:
Ex: 64-17-5'),
		'#required' => TRUE
	);
	$form['simulator_version_used'] = array(
			'#type' => 'select',
			'#title' => t('Simulator version used for creating the flowsheet'),
			'#options' => _df_list_of_simulator_version_used(),
			'#required' => TRUE,
		);
	
	$form['term_condition'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Terms And Conditions'),
		'#options' => array(
			'status' => t('<a href="/term-and-conditions" target="_blank">I agree to the Terms and Conditions</a>')
		),
		'#required' => TRUE
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}

/***************************************/
function om_flowsheet_proposal_form_validate($form, &$form_state)
{
	/*$project_title = $form_state['values']['project_title'];
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('project_title', $project_title);
	$query->condition(db_or()->condition('approval_status',1)->condition('approval_status',3)); 
	$result = $query->execute()->rowCount();
	if ($result > 0)
	{
		form_set_error('project_title', t('Project title name already exists'));
		return;
	}*/
	/*$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('project_title', $project_title);
	$query->condition('approval_status',3); 
	$result1 = $query->execute()->rowCount();
	if ($result1 > 0)
	{
		form_set_error('project_title', t('Project title name already exists'));
		return;
	}*/
	if ($form_state['values']['term_condition'] == '1')
	{
		form_set_error('term_condition', t('Please check the terms and conditions'));
		// $form_state['values']['country'] = $form_state['values']['other_country'];
	} //$form_state['values']['term_condition'] == '1'
	if ($form_state['values']['country'] == 'Others')
	{
		if ($form_state['values']['other_country'] == '')
		{
			form_set_error('other_country', t('Enter country name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_country'] == ''
		else
		{
			$form_state['values']['country'] = $form_state['values']['other_country'];
		}
		if ($form_state['values']['other_state'] == '')
		{
			form_set_error('other_state', t('Enter state name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_state'] == ''
		else
		{
			$form_state['values']['all_state'] = $form_state['values']['other_state'];
		}
		if ($form_state['values']['other_city'] == '')
		{
			form_set_error('other_city', t('Enter city name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_city'] == ''
		else
		{
			$form_state['values']['city'] = $form_state['values']['other_city'];
		}
	} //$form_state['values']['country'] == 'Others'
	else
	{
		if ($form_state['values']['country'] == '0')
		{
			form_set_error('country', t('Select country name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['country'] == ''
		if ($form_state['values']['all_state'] == '0')
		{
			form_set_error('all_state', t('Select state name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['all_state'] == ''
		if ($form_state['values']['city'] == '0')
		{
			form_set_error('city', t('Select city name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['city'] == ''
	}
	//Validation for project title
	if($form_state['values']['dwsim_flowsheet_check'] == 1)
	{
		$project_title = $form_state['values']['dwsim_flowsheet_name_dropdown'];
	}
	else{

		$project_title = $form_state['values']['project_title'];
	}
	//var_dump($project_title);die;
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('project_title', $project_title);
	$query->condition(db_or()->condition('approval_status',0)->condition('approval_status',1)->condition('approval_status',3)); 
	$result = $query->execute()->rowCount();
	//var_dump($result);die;
	if ($result >= 1)
	{
		form_set_error('', t('Project title name already exists'));
		return;
	}
	if ($form_state['values']['project_title'] != '')
	{
		if (strlen($form_state['values']['project_title']) > 250)
		{
			form_set_error('project_title', t('Maximum charater limit is 250 charaters only, please check the length of the project title'));
		} //strlen($form_state['values']['project_title']) > 250
		else if (strlen($form_state['values']['project_title']) < 10)
		{
			form_set_error('project_title', t('Minimum charater limit is 10 charaters, please check the length of the project title'));
			
		}
		else if(preg_match('/[\^£$%&*()}{@#~?><>.:;`|=_+¬]/', $form_state['values']['project_title']))
			form_set_error('project_title', t('Special characters are not allowed for project title'));
	 //strlen($form_state['values']['project_title']) < 10
	} //$form_state['values']['project_title'] != ''
	/*else
	{
		form_set_error('project_title', t('Project title shoud not be empty'));
	} */
	// validation for Name of compound for which process development is carried out
	$form_state['values']['process_development_compound_name'] = trim($form_state['values']['process_development_compound_name']);
	if ($form_state['values']['process_development_compound_name'] != '')
	{
		if (strlen($form_state['values']['process_development_compound_name']) >= 50)
		{
			form_set_error('process_development_compound_name', t('Maximum charater limit is 50 charaters only, please check the length'));
		} //strlen($form_state['values']['process_development_compound_name']) >= 50
		else if (strlen($form_state['values']['process_development_compound_name']) < 1)
		{
			form_set_error('process_development_compound_name', t('Minimum charater limit is 1 charaters, please check the length'));
		} //strlen($form_state['values']['process_development_compound_name']) < 1
	} //$form_state['values']['process_development_compound_name'] != ''
	else
	{
		form_set_error('process_development_compound_name', t('Field should not be empty'));
	}
	$form_state['values']['process_development_compound_cas_no'] = trim($form_state['values']['process_development_compound_cas_no']);
	if ($form_state['values']['process_development_compound_cas_no'] != '')
	{
		if (strlen($form_state['values']['process_development_compound_cas_no']) < 1)
		{
			form_set_error('process_development_compound_cas_no', t('Minimum charater limit is 1 charaters, please check the length'));
		} //strlen($form_state['values']['process_development_compound_cas_no']) < 1
	} //$form_state['values']['process_development_compound_cas_no'] != ''
	else
	{
		form_set_error('process_development_compound_cas_no', t('CAS number field should not be empty'));
	}
	if ($form_state['values']['version'] == 'Old version')
	{
		if ($form_state['values']['older'] == '')
		{
			form_set_error('older', t('Please provide valid version'));
		} //$form_state['values']['older'] == ''
	} //$form_state['values']['version'] == 'Old version'
	/*if ($form_state['values']['om_database_compound_name'])
	{
		$om_database_compound_name = implode("| ", $_POST['om_database_compound_name']);
		$form_state['values']['om_database_compound_name'] = trim($om_database_compound_name);
	} *///$form_state['values']['om_database_compound_name']
	return;
}
function om_flowsheet_proposal_form_submit($form, &$form_state)
{
	global $user;
	if (!$user->uid)
	{
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
	} //!$user->uid
	if ($form_state['values']['version'] == 'Old version')
	{
		$form_state['values']['version'] = trim($form_state['values']['older']);
	} //$form_state['values']['version'] == 'Old version'
	/* inserting the user proposal */
	$v = $form_state["values"];
	$simulator_version_used = $v['simulator_version_used'];
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('simulator_version_name', $simulator_version_used);
	$result = $query->execute()->fetchObject();
	$simulator_version_used_id = $result->id;
	if($form_state['values']['dwsim_flowsheet_check'] == 1)
	{
		$project_title = $v['dwsim_flowsheet_name_dropdown'];
		$dwsim_flowsheet_check = 1;
	}
	else{
		$dwsim_flowsheet_check = 0;
		$project_title = $v['project_title'];
	}

	//$project_title = trim($v['project_title']);
	$proposar_name = trim($v['name_title'] . ' ' . $v['contributor_name']);
	$university = trim($v['university']);
	$month_year_of_degree = $v['month_year_of_degree'];
	$directory_name = _om_df_dir_name($project_title, $proposar_name);
	$result = "INSERT INTO {om_flowsheet_proposal} 
    (
    uid, 
    approver_uid,
    name_title, 
    contributor_name,
    contact_no,
    month_year_of_degree, 
    university,
    city, 
    pincode, 
    state, 
    country,
    version, 
    project_guide_name,
    project_guide_email_id,
    project_guide_university,
    project_title, 
    dwsim_flowsheet_check,
    process_development_compound_name, 
    process_development_compound_cas_number,
    simulator_version_id,
    approval_status,
    is_completed, 
    dissapproval_reason,
    creation_date, 
    approval_date, 
    directory_name,
   	reference
    ) VALUES
    (
    :uid, 
    :approver_uid, 
    :name_title, 
    :contributor_name, 
    :contact_no,
    :month_year_of_degree, 
    :university, 
    :city, 
    :pincode, 
    :state, 
    :country,
    :version, 
    :project_guide_name,
    :project_guide_email_id,
    :project_guide_university,
    :project_title, 
    :dwsim_flowsheet_check,
    :process_development_compound_name, 
    :process_development_compound_cas_number,
    :simulator_version_id,
    :approval_status,
    :is_completed, 
    :dissapproval_reason,
    :creation_date, 
    :approval_date, 
    :directory_name,
    :reference
    )";

	$args = array(
		":uid" => $user->uid,
		":approver_uid" => 0,
		":name_title" => $v['name_title'],
		":contributor_name" => _om_df_sentence_case(trim($v['contributor_name'])),
		":contact_no" => $v['contributor_contact_no'],
		":month_year_of_degree" => $month_year_of_degree,
		":university" => trim($v['university']),
		":city" => $v['city'],
		":pincode" => $v['pincode'],
		":state" => $v['all_state'],
		":country" => $v['country'],
		":version" => $v['version'],
		":project_guide_name" => $v['project_guide_name'],
		":project_guide_email_id" => trim($v['project_guide_email_id']),
		":project_guide_university" => trim($v['project_guide_university']),
		":project_title" => $project_title,
		":dwsim_flowsheet_check" => $dwsim_flowsheet_check,
		//":dwsim_flowsheet_name" => $v['dwsim_flowsheet_name'],
		":process_development_compound_name" => _om_df_sentence_case($v['process_development_compound_name']),
		":process_development_compound_cas_number" => $v['process_development_compound_cas_no'],
		":simulator_version_id" => $simulator_version_used_id,
		":approval_status" => 0,
		":is_completed" => 0,
		":dissapproval_reason" => "NULL",
		":creation_date" => time(),
		":approval_date" => 0,
		":directory_name" => $directory_name,
		":reference" => $v['reference']
	);
	//var_dump($args);die;
	$proposal_id = db_query($result, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));

	if (!$proposal_id)
	{
		drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
		return;
	} //!$proposal_id
	/* sending email */
	$email_to = $user->mail;
	$form = variable_get('om_flowsheet_from_email', '');
	$bcc = variable_get('om_flowsheet_emails', '');
	$cc = variable_get('om_flowsheet_cc_emails', '');
	$params['om_flowsheet_proposal_received']['proposal_id'] = $proposal_id;
	$params['om_flowsheet_proposal_received']['user_id'] = $user->uid;
	$params['om_flowsheet_proposal_received']['headers'] = array(
		'From' => $form,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('om_flowsheet', 'om_flowsheet_proposal_received', $email_to, user_preferred_language($user), $params, $form, TRUE))
		drupal_set_message('Error sending email message.', 'error');
	drupal_set_message(t('We have received your OpenModelica Flowsheeting proposal. We will get back to you soon.'), 'status');
	drupal_goto('chemical/flowsheeting-project');
}
