<?php
// $Id$
function om_flowsheet_download_user_defined_compound()
{
	$proposal_id = arg(3);
	$root_path = om_flowsheet_document_path();
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$query->range(0, 1);
	$result = $query->execute();
	$om_flowsheet_user_compund_data = $result->fetchObject();
	$samplecodename = substr($om_flowsheet_user_compund_data->user_defined_compound_filepath, strrpos($om_flowsheet_user_compund_data->user_defined_compound_filepath, '/') + 1);
	header('Content-Type: txt/zip');
	header('Content-disposition: attachment; filename="' . $samplecodename . '"');
	header('Content-Length: ' . filesize($root_path . $om_flowsheet_user_compund_data->directory_name . '/' . $om_flowsheet_user_compund_data->user_defined_compound_filepath));
	ob_clean();
	readfile($root_path . $om_flowsheet_user_compund_data->directory_name . '/' . $om_flowsheet_user_compund_data->user_defined_compound_filepath);
}
function om_flowsheet_download_abstract_file() {
	$proposal_id = arg(4);
	$root_path = om_flowsheet_document_path();
	$query = db_select('om_flowsheet_submitted_abstracts_file');
	$query->fields('om_flowsheet_submitted_abstracts_file');
	$query->condition('proposal_id', $proposal_id);
	$query->condition('filetype', A);
	$result = $query->execute();
	$om_pssp_project_files = $result->fetchObject();
	//var_dump($om_pssp_project_files);die;
	$query1 = db_select('om_flowsheet_proposal');
	$query1->fields('om_flowsheet_proposal');
	$query1->condition('id', $proposal_id);
	$result1 = $query1->execute();
	$om_pssp = $result1->fetchObject();
	$directory_name = $om_pssp->directory_name . '/';
	$samplecodename = $om_pssp_project_files->filename;
	ob_clean();
	header("Pragma: public");
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
	header("Cache-Control: public");
	header("Content-Description: File Transfer");
	header("Content-Type: application/pdf");
	header('Content-disposition: attachment; filename="' . $samplecodename . '"');
	header("Content-Length: " . filesize($root_path . $directory_name . $samplecodename));
	header("Content-Transfer-Encoding: binary");
	header("Expires: 0");
	header("Pragma: no-cache");
	readfile($root_path . $directory_name . $samplecodename);
	ob_end_flush();
	ob_clean();
}