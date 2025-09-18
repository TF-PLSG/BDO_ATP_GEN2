<?php
class ExportExcelSheet {
	public static function exportExcel($result, $headers, $filename='excel', $option = array()){
		include('PHPExcel/Classes/PHPExcel.php');
		$objPHPExcel = new PHPExcel();
		$objPHPExcel->getProperties()
			->setCreator("BDO")
			->setLastModifiedBy("BDO")
			->setTitle("BDO-Excel")
			->setSubject("BDO-Excel")
			->setDescription("BDO-Excel.")
			->setKeywords("BDO")
			->setCategory("BDO");
		
		$objPHPExcel->setActiveSheetIndex(0);
		$worksheet = $objPHPExcel->getActiveSheet();
		$first_letter = PHPExcel_Cell::stringFromColumnIndex(0);
		$last_letter = PHPExcel_Cell::stringFromColumnIndex(count($headers)-1);
		$header_range = "{$first_letter}1:{$last_letter}1";
		$objPHPExcel->getActiveSheet()->getStyle($header_range)->getFont()->setBold(true);
		
		if(!empty($result)) {
			$rowCnt =1;
			$col =0;
			
			foreach($headers as $key=>$header) {
				$worksheet->setCellValueByColumnAndRow($col, $rowCnt, $header['external']);
				$col++;
			}
	
			$col=0;
			foreach($result as $row) {
				$rowCnt++;
				$col=0;
				foreach($headers as $key=>$header) {
					$worksheet->getCellByColumnAndRow($col, $rowCnt)->getStyle()->getFont()->setBold(false);
					$cellValue  = (isset($row[$header['internal']]) && !empty($row[$header['internal']])) ? $row[$header['internal']]:'-';
					/*if($header['internal'] == 'is_active') {
						$cellValue  = $row[$header['internal']] == 1 ? 'Yes':'No';
						
					}
					if(stripos($header['internal'], 'date') !== false) {
						if(isset($option['string_date']) && $option['string_date']) {
							$row[$header['internal']] = strtotime($row[$header['internal']]);
						}
						$cellValue  = date('d-m-Y', $row[$header['internal']]);
					}*/
					//echo $rowCnt.'|'.$col.'|'.$cellValue;echo '<br>';
					$worksheet->setCellValueByColumnAndRow($col, $rowCnt, $cellValue );
					$col++;
				}
				if($rowCnt > 10) break;
			}
		} else {
			return false;
		}
		
		$objPHPExcel->setActiveSheetIndex(0);
		// Redirect output to a client’s web browser (Excel5)
		header('Content-Type: application/vnd.ms-excel');
		header('Content-Disposition: attachment;filename="'.$filename.'.xls"');
		header('Cache-Control: max-age=0');
		// If you're serving to IE 9, then the following may be needed
		header('Cache-Control: max-age=1');
		
		// If you're serving to IE over SSL, then the following may be needed
		header ('Expires: Mon, 26 Jul 1997 05:00:00 GMT'); // Date in the past
		header ('Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT'); // always modified
		header ('Cache-Control: cache, must-revalidate'); // HTTP/1.1
		header ('Pragma: public'); // HTTP/1.0
		
		$objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
		$objWriter->save('php://output');
		exit;
	}
	
	/*public static GetEncryption(PlainTextPassword)
    {
		try
		{
			uint h = 0;
			uint g = 3435973836;
			foreach (char ch in PlainTextPassword)
			{
				h = (h << 4) + ch;
				g = h & 0xF0000000;
				if (Convert.ToBoolean(g))
				{
					h ^= (g >> 24);
				}

				h &= ~g;

			}

			return h.ToString().PadLeft(32, '0');
		}
		catch (Exception)
		{

			throw;
		}

    }*/
}
?>