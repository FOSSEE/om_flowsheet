<?php
// om Flowsheet display completed proposals
function om_flowsheet_completed_proposals_all()
{
	$output = "";
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('actual_completion_date', 'DESC');
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$output .= "Work has been completed for the following flow sheets. We welcome your contributions. For more details, please visit ".l("https://om.fossee.in/chemical/flowsheeting-project","https://om.fossee.in/chemical/flowsheeting-project", array('attributes' => array('target' => '_blank')))."<hr>";

	} //$result->rowCount() == 0
	else
	{
		$output .= "Work has been completed for the following flow sheets. We welcome your contributions. For more details, please visit ".l("https://om.fossee.in/chemical/flowsheeting-project","https://om.fossee.in/chemical/flowsheeting-project", array('attributes' => array('target' => '_blank')))."<hr>";
		$preference_rows = array();
		$i = $result->rowCount();
		while ($row = $result->fetchObject())
		{
			$completion_date = date("Y", $row->actual_completion_date);
			$preference_rows[] = array(
				$i,
				l($row->project_title, "chemical/flowsheeting-project/om-flowsheet-run/" . $row->id, array('attributes' => array('title' => 'This is a zip file containing a pdf (abstract) and a mo file which is the OpenModelica flowsheet which is to be viewed by right clicking on the file and opening with OpenModelica.'))),
				$row->contributor_name,
				$row->university,
				$completion_date
			);
			$i--;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Flowsheet Project',
			'Contributor Name',
			'University / Institute',
			'Year of Completion'
		);
		$output .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $output;
}
// om Flowsheet display in progress proposals
function om_flowsheet_progress_all()
{
	$page_content = "";
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 1);
	$query->condition('is_completed', 0);
	$query->orderBy('approval_date', DESC);
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$page_content .= "Work is in progress for the following flowsheets under Flowsheeting Project<hr>";
	} //$result->rowCount() == 0
	else
	{
		$page_content .= "Work is in progress for the following flowsheets under Flowsheeting Project<hr>";;
		$preference_rows = array();
		$i = $result->rowCount();
		while ($row = $result->fetchObject())
		{
			$approval_date = date("Y", $row->approval_date);
			$preference_rows[] = array(
				$i,
				$row->project_title,
				$row->contributor_name,
				$row->university,
				$approval_date
			);
			$i--;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Flowsheet Project',
			'Contributor Name',
			'University / Institute',
			'Year'
		);
		$page_content .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $page_content;
}
