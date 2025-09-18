<?php

/**
 * @package		MVC
 * @author		Gopal Sikhawal
 */
define('NUM_RESULT_PER_PAGE', 25);
class Tlf_Model extends MVC_Model
{

	function getAllTransactionLookUpExport($data) {
		
		$param = array();
		unset($data['UNMASK']);
		
		//$where = " WHERE DATE_YYYYMMDD BETWEEN TO_DATE('".$data['SYSTEM_DATE_START']."', 'SYYYY-MM-DD HH24:MI:SS') AND TO_DATE('".$data['SYSTEM_DATE_END']."', 'SYYYY-MM-DD HH24:MI:SS') ";
		$where = " WHERE SYSTEM_DATE>='".str_replace('-', '', $data['SYSTEM_DATE_START'])."' AND SYSTEM_DATE <='".str_replace('-', '', $data['SYSTEM_DATE_END'])."'";
		unset($data['SYSTEM_DATE_START']);
		unset($data['SYSTEM_DATE_END']);
		
		foreach($data as $key => $value) {
			if($value) {
				if($key == 'TRANSACTION_START_TIME') {
					$param[$key] = $value.'00';
					$where .= ' AND TIME_HHMMSS >= :b'.$key;	
				} elseif($key == 'TRANSACTION_END_TIME') {
					$param[$key] = $value.'59';
					$where .= ' AND TIME_HHMMSS <= :b'.$key;
				} else {
					$param[$key] = $value.'%';
					$where .= ' AND '.$key.' LIKE :b'.$key;
				}				
			}
		}
		$result = array();
		
		$sql = 'SELECT * FROM TLF01_ARCHIVE '.$where.' ORDER BY TRANSACTION_ID DESC';

		$result = database::doSelect($sql, $param);
		
        return $result;
	}
    function getAllTransactionLookUp($page, $data) {
		$param = array();
		$first = true;
		$where = '';
		unset($data['UNMASK']);
		//echo '<pre>';print_r($data);exit;
		//$where = " WHERE DATE_YYYYMMDD BETWEEN TO_DATE('".$data['SYSTEM_DATE_START']."', 'SYYYY-MM-DD HH24:MI:SS') AND TO_DATE('".$data['SYSTEM_DATE_END']."', 'SYYYY-MM-DD HH24:MI:SS') ";
		$where = " WHERE SYSTEM_DATE>='".str_replace('-', '', $data['SYSTEM_DATE_START'])."' AND SYSTEM_DATE <='".str_replace('-', '', $data['SYSTEM_DATE_END'])."'";
		unset($data['SYSTEM_DATE_START']);
		unset($data['SYSTEM_DATE_END']);
		
		
		foreach($data as $key => $value) {
			if($value) {
				if($key == 'TRANSACTION_START_TIME') {
					$param[$key] = $value.'00';
					$where .= ' AND TIME_HHMMSS >= :b'.$key;	
				} elseif($key == 'TRANSACTION_END_TIME') {
					$param[$key] = $value.'59';
					$where .= ' AND TIME_HHMMSS <= :b'.$key;
				} else {
					$param[$key] = $value.'%';
					$where .= ' AND '.$key.' LIKE :b'.$key;
				}				
			}
		}
        $start = ($page-1)*NUM_RESULT_PER_PAGE;
        $end = $start+NUM_RESULT_PER_PAGE;

		$sql = 'SELECT count(*) AS TOTAL FROM TLF01_ARCHIVE '.$where;
		$count = database::doSelectOne($sql, $param);
		
		$result = array();
		$result['Total'] = isset($count['TOTAL']) ? $count['TOTAL'] : 0;
		$result['NUM_RESULT_PER_PAGE'] = NUM_RESULT_PER_PAGE;
		
		if(isset($count['TOTAL']) && $count['TOTAL'] > $start) {
			$sql = 'SELECT O.* FROM
						(SELECT ROWNUM RN, I.*  FROM
							(SELECT * FROM TLF01_ARCHIVE '.$where.' ORDER BY TRANSACTION_ID DESC ) I
						) O
					WHERE O.RN > '.$start.' AND O.RN <='.$end;

			$result['data'] = database::doSelect($sql, $param);
			
		}
		
        return $result;
    }

    
    public function getTransactionInfo($TRANSACTION_ID, $SYSTEM_DATE) {
        $param['TRANSACTION_ID'] = $TRANSACTION_ID;
        $param['SYSTEM_DATE'] = $SYSTEM_DATE;
		
      

		  $where = ' WHERE TRANSACTION_ID = :bTRANSACTION_ID AND SYSTEM_DATE = :bSYSTEM_DATE';
       $sql = 'SELECT * FROM TLF01_ARCHIVE '.$where;

      
        $data = database::doSelectOne($sql, $param);
        return $data;
    }
	
	    public function getMRAInfo($TRANSACTION_ID, $SYSTEM_DATE) {
        $param['TRANSACTION_ID'] = $TRANSACTION_ID;
        $param['SYSTEM_DATE'] = $SYSTEM_DATE;
		

		$where = ' WHERE TRANSACTION_ID = :bTRANSACTION_ID AND SYSTEM_DATE = :bSYSTEM_DATE' ;
        $sql = 'SELECT * FROM MRA01_ARCHIVE '.$where;

	   
        $data = database::doSelectOne($sql, $param);
		
        return $data;
    }
	
	function getAllHstLookUpExport($data) {

		$param = array();
		$first = true;
		$where = '';
		if(isset($data['SYSTEM_DATE_START']) && $data['SYSTEM_DATE_START']) {
			$data['SYSTEM_DATE_START'] = date('Ymd', strtotime($data['SYSTEM_DATE_START']));
		}	
		if(isset($data['SYSTEM_DATE_END']) && $data['SYSTEM_DATE_END']) {
			$data['SYSTEM_DATE_END'] = date('Ymd', strtotime($data['SYSTEM_DATE_END']));
		}
		foreach($data as $key => $value) {
			if($value) {
				
				if($first) {
					$first = false;
					if($key == 'SYSTEM_DATE_START') {
						$param[$key] = $value;
						$where = ' WHERE DATE_MODIFIED =:b'.$key;
					} elseif($key == 'SYSTEM_DATE_END') {
						$param[$key] = $value;
						$where = ' WHERE DATE_MODIFIED <=:b'.$key;
					} else {
						$param[$key] = $value.'%';
						$where = ' WHERE '.$key.' LIKE :b'.$key;
					}
				} else {
					if($key == 'SYSTEM_DATE_START') {
						$param[$key] = $value;
						$where .= ' AND DATE_MODIFIED =:b'.$key;
					} elseif($key == 'SYSTEM_DATE_END') {
						$param[$key] = $value;
						$where .= ' AND DATE_MODIFIED <=:b'.$key;
					} else {
						$param[$key] = $value.'%';
						$where .= ' AND '.$key.' LIKE :b'.$key;
					}
				}
			}
		}
        
		$sql = 'SELECT * FROM HST01 '.$where;
		$result = database::doSelect($sql, $param, database::SECOND_INSTANCE);
		
        return $result;
    }

	function getAllHstLookUp($page, $data) {

		$param = array();
		$first = true;
		$where = '';
		if(isset($data['SYSTEM_DATE_START']) && $data['SYSTEM_DATE_START']) {
			$data['SYSTEM_DATE_START'] = date('Ymd', strtotime($data['SYSTEM_DATE_START']));
		}	
		if(isset($data['SYSTEM_DATE_END']) && $data['SYSTEM_DATE_END']) {
			$data['SYSTEM_DATE_END'] = date('Ymd', strtotime($data['SYSTEM_DATE_END']));
		}
		foreach($data as $key => $value) {
			if($value) {
				
				if($first) {
					$first = false;
					if($key == 'SYSTEM_DATE_START') {
						$param[$key] = $value;
						$where = ' WHERE DATE_MODIFIED =:b'.$key;
					} elseif($key == 'SYSTEM_DATE_END') {
						$param[$key] = $value;
						$where = ' WHERE DATE_MODIFIED <=:b'.$key;
					} else {
						$param[$key] = $value.'%';
						$where = ' WHERE '.$key.' LIKE :b'.$key;
					}
				} else {
					if($key == 'SYSTEM_DATE_START') {
						$param[$key] = $value;
						$where .= ' AND DATE_MODIFIED =:b'.$key;
					} elseif($key == 'SYSTEM_DATE_END') {
						$param[$key] = $value;
						$where .= ' AND DATE_MODIFIED <=:b'.$key;
					} else {
						$param[$key] = $value.'%';
						$where .= ' AND '.$key.' LIKE :b'.$key;
					}
				}
			}
		}
        $start = ($page-1)*NUM_RESULT_PER_PAGE;
        $end = $start+NUM_RESULT_PER_PAGE;

		$sql = 'SELECT count(*) AS TOTAL FROM HST01 '.$where;
		$count = database::doSelectOne($sql, $param, database::SECOND_INSTANCE);
		
		$result = array();
		$result['Total'] = isset($count['TOTAL']) ? $count['TOTAL'] : 0;
		$result['NUM_RESULT_PER_PAGE'] = NUM_RESULT_PER_PAGE;
		
		if(isset($count['TOTAL']) && $count['TOTAL'] > $start) {
			$sql = 'SELECT O.* FROM
						(SELECT ROWNUM RN, I.*  FROM
							(SELECT * FROM HST01 '.$where.' ORDER BY UNIQUE_ID DESC ) I
						) O
					WHERE O.RN > '.$start.' AND O.RN <='.$end;

			$result['data'] = database::doSelect($sql, $param, database::SECOND_INSTANCE);
		}
		
        return $result;
    }
	function HstLookUp($param) {
        $where = '';
        
		$sql = 'SELECT * FROM HST01 '.$where;
		return database::doSelect($sql, $param, database::SECOND_INSTANCE);
		
    }
    
    public function getHstInfo($UNIQUE_ID) {
        $param['UNIQUE_ID'] = $UNIQUE_ID;
        $where = ' WHERE UNIQUE_ID = :bUNIQUE_ID';
        $sql = 'SELECT * FROM HST01 '.$where;
        
        $data = database::doSelectOne($sql, $param, database::SECOND_INSTANCE);
        return $data;
    }
	public function UpdateHST($param) {
		//echo '<pre>';print_r($param);exit;
		$first = true;
		$keys = '';
		$values = '';
		foreach($param as $k => $v) {
			if($first) {
				$first = false;
				$keys .= $k;
				$values .= ':b'.$k;
			} else {
				$keys = $keys.', '.$k;
				$values = $values.', :b'.$k;
			}
		}
		
		$sql = 'INSERT INTO HST01 ('.$keys.') VALUES ('.$values.')';
	//	echo $sql;exit;
		$data = database::doInsert($sql, $param, database::SECOND_INSTANCE);
        return $data;
	}
	public function exportTransactionCsv($type, $columns) {
		ini_set('memory_limit', '-1');
		$type = trim($type);
		$result = array();
		
		if($type == 'transaction') {
			$filename="TLF01Export.csv";
			$fname = 'TLF01';
			$ftag = 'TLF01';
			
			$expColumns = $this->getTLFExportColumns();
			$result = $this->getAllTransactionLookUpExport($_SESSION[$type]);
			foreach($result as $k => $v) {
				$result[$k] = Transaction_Model::processed_data($v);
			}
			
		} /*elseif($type == 'mra')
		{
			$filename="MRA01Export.csv";
			$fname = 'MRA01';
			$ftag = 'MRA01';
			$expColumns = $this->getMRAExportColumns();
			$result = $this->getAllMRALookUpExport($_SESSION[$type]);
		}*/
		elseif($type == 'hst') {
			
			$filename="HST01Export.csv";
			$fname = 'HST01';
			$ftag = 'HST01';
			$expColumns = $this->getHSTExportColumns();
			$result = $this->getAllHstLookUpExport($_SESSION[$type]);
			
		} 
		
		if(isset($_SESSION[$type]) && !empty($_SESSION[$type])) {
			if(isset($_SESSION['export_user'])) {
				$UNIQUE_ID = date('YmdHis');
				$mt = microtime(true);
				$s = explode('.',$mt);
				$micro = $s[1]*1000;
				$UNIQUE_ID .= substr($micro, 0, 3);
			
				$hst_data = array('CHANGED_BY' => $_SESSION['export_user']['NAME'], 
					'FILE_NAME' => $fname, 
					'FIELD_TAG' => $ftag,
					'FILE_PRIMARY_KEY' => 'ARCHIVE',
					'DATE_MODIFIED' => date('Ymd'),
					'TIME_MODIFIED' => date('His'),
					'UNIQUE_ID' => $UNIQUE_ID,
					'CURRENT_DATA' => $filename
				);
				$this->UpdateHST($hst_data);
			}
		}
		
		ob_start();
		header('Content-Type: application/excel');
		header('Content-Disposition: attachment; filename="'.$filename.'"');
		header('Cache-Control: max-age=0');
		$fp = fopen('php://output', 'w');
		$first = true;
		
		foreach ($result as $line ) {
			//echo '<pre>';print_r($line);print_r($columns);echo '</pre>';exit;
			if($first) {
				$first = false;
				$header = array();
				foreach($columns as $c) {
					$header[] = $expColumns[$c];
				}
				fputcsv($fp, $header);
			}
			
			$temp = array();
			foreach($columns as $key) {
				
				//$val = isset($line[$key]) ? $line[$key] : 'Gopal';
				$val = $line[$key];
				if($key == 'CARD_NUM') {
					if(isset($_SESSION[$type]['UNMASK']) && $_SESSION[$type]['UNMASK'] == 1) {
						$temp[] = $val;
					} elseif(!empty($val)) {
						$temp[] = substr($val, 0, 6) . str_repeat("X", strlen($val) - 10) . substr($val, -4);
					}
				} else {
					$temp[] = $val;
				}
				
			}
			unset($key);unset($val);
			fputcsv($fp, $temp);
		}
		fclose($fp);
		$out = ob_get_contents();
		ob_end_clean();
		echo trim($out);
		//exit;
	}
		/*public function getMRAExportColumns() {
		$expColumns = array();
		$expColumns['TRANSACTION_ID'] = 'TRANSACTION_ID';
		$expColumns['SYSTEM_DATE'] = 'SYSTEM_DATE';
		$expColumns['TRANSACTION_SEND_TIME'] = 'TRANSACTION_SEND_TIME';
		$expColumns['MRACON_SENDER_NAME'] = 'MRACON_SENDER_NAME';
		$expColumns['TRANSACTION_NUMBER'] = 'TRANSACTION_NUMBER';
		$expColumns['PURGE_DATE'] = 'Purge Date';
		return $expColumns;
	}*/
	public function getHSTExportColumns() {
		$expColumns = array();
		$expColumns['UNIQUE_ID'] = 'UNIQUE_ID';
		$expColumns['FILE_NAME'] = 'FILE_NAME';
		$expColumns['FILE_PRIMARY_KEY'] = 'FILE_PRIMARY_KEY';
		$expColumns['CHANGED_BY'] = 'CHANGED_BY';
		$expColumns['DATE_MODIFIED'] = 'DATE_MODIFIED';
		$expColumns['TIME_MODIFIED'] = 'TIME_MODIFIED';
		$expColumns['FIELD_TAG'] = 'FIELD_TAG';
		$expColumns['PREVIOUS_DATA'] = 'PREVIOUS_DATA';
		$expColumns['CURRENT_DATA'] = 'CURRENT_DATA';
		$expColumns['RESERVED'] = 'RESERVED';
		return $expColumns;
	}
	public function getTLFExportColumns() {
		$expColumns = array();

		$expColumns['TransactionType'] = 'TRANSACTION_TYPE';
		$expColumns['RETRIEVAL_REF_NUM'] = 'RETRIEVAL_REF_NUM';
		$expColumns['PROCESSING_CODE'] = 'PROCESSING_CODE';
		$expColumns['TRANSACTION_ID'] = 'TRANSACTION_ID';
		
		$expColumns['CARD_NUM'] = 'CARD_NUM';
		$expColumns['TOTAL_AMOUNT'] = 'TOTAL_AMOUNT';
		$expColumns['CURRENCY_CODE'] = 'CURRENCY_CODE';
		$expColumns['ENTRY_TYPE'] = 'ENTRY_TYPE';
		
		$expColumns['RESPONSE_CODE'] = 'RESPONSE_CODE';
		$expColumns['AUTH_NUMBER'] = 'AUTH_NUMBER';
		$expColumns['TERMINAL_ID'] = 'DEVICE_ID';
		$expColumns['TX_KEY'] = 'TX_KEY';
		$expColumns['HOST_NAME'] = 'REDEMPTION_AMOUNT';
		
		$expColumns['MERCHANT_ID'] = 'MERCHANT_ID';
		$expColumns['CATEGORY_CODE'] = 'CATEGORY_CODE';
		$expColumns['SYS_TRACE_AUDIT_NUM'] = 'SYS_TRACE_AUDIT_NUM';
		$expColumns['TERMINAL_TYPE'] = 'TERMINAL_TYPE';
		
		$expColumns['SERVICE_CODE'] = 'SERVICE_CODE';
		$expColumns['ORG_TXN_AMT'] = 'ORG_TXN_AMT';
		$expColumns['ORG_TXN_CUR'] = 'ORG_TXN_CUR';
		$expColumns['CARD_NUM_LEN'] = 'CARD_NUM_LEN';
		
		$expColumns['INVOICE_NUMBER'] = 'INVOICE_NUMBER';
		$expColumns['DATE_YYYYMMDD'] = 'TRAN_DATE';
		$expColumns['TIME_HHMMSS'] = 'TRAN_TIME';
		$expColumns['EXP_DATE'] = 'EXP_DATE';
		
		$expColumns['POS_ENTRY_MODE'] = 'POS_ENTRY_MODE';
		$expColumns['RESPONSE_TEXT'] = 'RESPONSE_TEXT'; 
		$expColumns['POS_CONDITION_CODE'] = 'POS_CONDITION_CODE';
		$expColumns['ADD_AMOUNTS'] = 'ADD_AMOUNTS';
		
		$expColumns['PRODUCT17_AMOUNT'] = 'POS_DATA';
		$expColumns['ACQUIRING_COUNTRY'] = 'ACQUIRING_COUNTRY';
		$expColumns['EarMarkedAmt'] = 'EarmarkedAmt';
		$expColumns['ORIG_AMOUNT'] = 'ORIG_AMOUNT';
		
		$expColumns['EarMarkedCurrency'] = 'EarmarkedCurrency';
		$expColumns['ACQUIRER_ID'] = 'ACQUIRER_ID';
		$expColumns['ISSUER_ID'] = 'ISSUER_ID';
		$expColumns['ORIG_SYS_TRACE_NUM'] = 'ORIG_SYS_TRACE_NUM';
		
		$expColumns['NII'] = 'NII';
		
		//$expColumns['TRACK1'] = 'TRACK1';
		//$expColumns['TRACK2'] = 'TRACK2';
		
		$expColumns['ORIG_RETRIEVAL_REF_NUM'] = 'ORIG_RETRIEVAL_REF_NUM';
		
		//$expColumns['ORIG_MESSAGE'] = 'ORIG_MESSAGE';
		$expColumns['SETTLEMENT_TOTAL'] = 'SETTLEMENT_TOTAL';
		$expColumns['MESSAGE_TYPE'] = 'MESSAGE_TYPE';
		$expColumns['CARD_HOLDER_NAME'] = 'MERCHANT_NAME';
		$expColumns['GENERAL_STATUS'] = 'GENERAL_STATUS';
		$expColumns['ORIGINATOR_QUEUE'] = 'ORIGINATOR_QUEUE';
		$expColumns['ORIGINATOR_INFO'] = 'ORIGINATOR_INFO';
		$expColumns['BATCH_NUMBER'] = 'BATCH_NUMBER';
		
		$expColumns['NUM_SALES'] = 'NUM_SALES';
		$expColumns['SALES_AMOUNT'] = 'SALES_AMOUNT'; 
		$expColumns['NUM_REFUNDS'] = 'NUM_REFUNDS';
		$expColumns['REFUND_AMOUNT'] = 'REFUND_AMOUNT';
		$expColumns['NUM_DBSALES'] = 'NUM_DBSALES';
		$expColumns['DBSALES_AMOUNT'] = 'DBSALES_AMOUNT';
		$expColumns['NUM_DBREFUNDS'] = 'NUM_DBREFUNDS';
		$expColumns['DBREFUND_AMOUNT'] = 'DBREFUND_AMOUNT'; 
		//$expColumns['PIN_BLOCK'] = 'PIN_BLOCK';
		//$expColumns['TAX_AMOUNT'] = 'TAX_AMOUNT';
		$expColumns['TIP_AMOUNT'] = 'TIP_AMOUNT';
		$expColumns['TRAN_START_TIME'] = 'TRAN_START_TIME';
		$expColumns['HOST_START_TIME'] = 'HOST_START_TIME';
		$expColumns['HOST_FINISH_TIME'] = 'HOST_FINISH_TIME';
		$expColumns['TRAN_FINISH_TIME'] = 'TRAN_FINISH_TIME';
		$expColumns['SETTLE_FILE_PREFIX'] = 'SETTLE_FILE_PREFIX';
		//$expColumns['RESP_SOURCE_LEN'] = 'RESP_SOURCE_LEN';
		$expColumns['VISA_CHAR_INDICATOR'] = 'VISA_CHAR_INDICATOR';
		$expColumns['VISA_TRAN_ID'] = 'VISA_TRAN_ID';
		$expColumns['VISA_VALIDATION_CODE'] = 'VISA_VALIDATION_CODE';
		//$expColumns['MCARD_BANKNET_LEN'] = 'MCARD_BANKNET_LEN';
		$expColumns['MCARD_BANKNET'] = 'MCARD_BANKNET';
		$expColumns['AGENT_ID_LEN'] = 'AGENT_ID_LEN';
		$expColumns['AGENT_ID'] = 'AGENT_ID';
		//$expColumns['CVC'] = 'CVC';
		$expColumns['SETTLEMENT_DATE'] = 'SETTLEMENT_DATE';
		
		$expColumns['CONVERSION_RATE'] = 'CONVERSION_RATE';
		$expColumns['PRODUCT_CODE'] = 'PRODUCT_CODE';
		//$expColumns['ODOMETER'] = 'ODOMETER';
		$expColumns['ORGANIZATION_ID'] = 'ORGANIZATION_ID';
		//$expColumns['DYNAMIC_KEYCHANGE'] = 'DYNAMIC_KEYCHANGE';
		//$expColumns['SOURCE_KEY'] = 'SOURCE_KEY';
		$expColumns['TXN_CNT'] = 'TXN_CNT';
		//$expColumns['DCF01_RETIRED_CWK'] = 'DCF01_RETIRED_CWK';
		//$expColumns['DEST_KEY'] = 'DEST_KEY';
		//$expColumns['NCF01_RETIRED_CWK'] = 'NCF01_RETIRED_CWK';
		$expColumns['HANDLER_QUEUE'] = 'HANDLER_QUEUE';
		$expColumns['AUTHORIZING_HOST_QUEUE'] = 'AUTHORIZING_HOST_QUEUE';
		//$expColumns['MAX_NET_CONSEC_TMOUTS'] = 'MAX_NET_CONSEC_TMOUTS'; 
		$expColumns['MAX_ACTIVE_TXNS'] = 'MAX_ACTIVE_TXNS';
		$expColumns['FORWARDING_INSTITUTION_ID'] = 'FORWARDING_INSTITUTION_ID';
		//$expColumns['REMOTE_NSP_TYPE'] = 'REMOTE_NSP_TYPE';
		//$expColumns['KEK'] = 'KEK';
		$expColumns['ACQUIRING_ID'] = 'ACQUIRING_ID';
		$expColumns['OUTSTANDING_BALANCE'] = 'OUTSTANDING_BALANCE';
		$expColumns['CREDIT_LIMIT'] = 'CREDIT_LIMIT';
		$expColumns['NFI_NAME'] = 'NFI_NAME';
		$expColumns['NFI_SEQ_NBR'] = 'NFI_SEQ_NBR';
		//$expColumns['NFI_START_DATE'] = 'NFI_START_DATE';
		$expColumns['NFI_END_DATE'] = 'NFI_END_DATE';
		$expColumns['RESP_SOURCE'] = 'RESP_SOURCE'; 
		$expColumns['ORIGIN'] = 'ORIGIN';
		$expColumns['AUTH_1'] = 'AUTH_1';
		$expColumns['AUTH_2'] = 'AUTH_2';
		$expColumns['UPDATE_MASK'] = 'UPDATE_MASK';
		$expColumns['TYPE_OF_DATA'] = 'TYPE_OF_DATA';
		$expColumns['DOWN_PAYMENT'] = 'DOWN_PAYMENT';
		$expColumns['PERIOD'] = 'PERIOD';
		$expColumns['INTEREST_RATE'] = 'INTEREST_RATE';
		$expColumns['TOTAL_INTEREST'] = 'TOTAL_INTEREST';
		$expColumns['REDEMPTION_AMOUNT'] = 'REDEMPTION_AMOUNT';
		$expColumns['VEHICLE_NUMBER'] = 'VEHICLE_NUMBER';
		$expColumns['PROFILE'] = 'PROFILE';
		$expColumns['NBR_OF_PROD_CODES'] = 'NBR_OF_PROD_CODES';
		/*$expColumns['PRODUCT1_CODE'] = 'PRODUCT1_CODE';
		$expColumns['PRODUCT1_QUANTITY'] = 'PRODUCT1_QUANTITY';
		$expColumns['PRODUCT1_AMOUNT'] = 'PRODUCT1_AMOUNT';
		$expColumns['PRODUCT2_CODE'] = 'PRODUCT2_CODE';
		$expColumns['PRODUCT2_QUANTITY'] = 'PRODUCT2_QUANTITY';
		$expColumns['PRODUCT2_AMOUNT'] = 'PRODUCT2_AMOUNT';
		$expColumns['PRODUCT3_CODE'] = 'PRODUCT3_CODE';
		$expColumns['PRODUCT3_QUANTITY'] = 'PRODUCT3_QUANTITY';
		$expColumns['PRODUCT3_AMOUNT'] = 'PRODUCT3_AMOUNT';
		$expColumns['PRODUCT4_CODE'] = 'PRODUCT4_CODE';
		$expColumns['PRODUCT4_QUANTITY'] = 'PRODUCT4_QUANTITY'; 
		$expColumns['PRODUCT4_AMOUNT'] = 'PRODUCT4_AMOUNT';*/
		$expColumns['PRODUCT5_CODE'] = 'PRODUCT5_CODE';
		$expColumns['PRODUCT5_QUANTITY'] = 'PRODUCT5_QUANTITY';
		$expColumns['PRODUCT5_AMOUNT'] = 'PRODUCT5_AMOUNT';
		/*$expColumns['PRODUCT6_CODE'] = 'PRODUCT6_CODE';
		$expColumns['PRODUCT6_QUANTITY'] = 'PRODUCT6_QUANTITY';
		$expColumns['PRODUCT6_AMOUNT'] = 'PRODUCT6_AMOUNT';
		$expColumns['PRODUCT7_CODE'] = 'PRODUCT7_CODE';
		$expColumns['PRODUCT7_QUANTITY'] = 'PRODUCT7_QUANTITY';
		$expColumns['PRODUCT7_AMOUNT'] = 'PRODUCT7_AMOUNT';
		$expColumns['PRODUCT8_CODE'] = 'PRODUCT8_CODE';
		$expColumns['PRODUCT8_QUANTITY'] = 'PRODUCT8_QUANTITY';
		$expColumns['PRODUCT8_AMOUNT'] = 'PRODUCT8_AMOUNT';
		$expColumns['PRODUCT9_CODE'] = 'PRODUCT9_CODE';
		$expColumns['PRODUCT9_QUANTITY'] = 'PRODUCT9_QUANTITY';
		$expColumns['PRODUCT9_AMOUNT'] = 'PRODUCT9_AMOUNT';
		$expColumns['PRODUCT10_CODE'] = 'PRODUCT10_CODE';
		$expColumns['PRODUCT10_QUANTITY'] = 'PRODUCT10_QUANTITY';
		$expColumns['PRODUCT10_AMOUNT'] = 'PRODUCT10_AMOUNT';
		$expColumns['PRODUCT11_CODE'] = 'PRODUCT11_CODE';
		$expColumns['PRODUCT11_QUANTITY'] = 'PRODUCT11_QUANTITY';
		$expColumns['PRODUCT11_AMOUNT'] = 'PRODUCT11_AMOUNT';
		$expColumns['PRODUCT12_CODE'] = 'PRODUCT12_CODE';
		$expColumns['PRODUCT12_QUANTITY'] = 'PRODUCT12_QUANTITY';
		$expColumns['PRODUCT12_AMOUNT'] = 'PRODUCT12_AMOUNT';*/
		//$expColumns['PRODUCT13_CODE'] = 'PRODUCT13_CODE';
		/*$expColumns['PRODUCT13_QUANTITY'] = 'PRODUCT13_QUANTITY';
		$expColumns['PRODUCT13_AMOUNT'] = 'PRODUCT13_AMOUNT';
		$expColumns['PRODUCT14_CODE'] = 'PRODUCT14_CODE';
		$expColumns['PRODUCT14_QUANTITY'] = 'PRODUCT14_QUANTITY';
		$expColumns['PRODUCT14_AMOUNT'] = 'PRODUCT14_AMOUNT';
		$expColumns['PRODUCT15_CODE'] = 'PRODUCT15_CODE';
		$expColumns['PRODUCT15_QUANTITY'] = 'PRODUCT15_QUANTITY';
		$expColumns['PRODUCT15_AMOUNT'] = 'PRODUCT15_AMOUNT';
		$expColumns['PRODUCT16_CODE'] = 'PRODUCT16_CODE';
		$expColumns['PRODUCT16_QUANTITY'] = 'PRODUCT16_QUANTITY';
		$expColumns['PRODUCT16_AMOUNT'] = 'PRODUCT16_AMOUNT';
		$expColumns['PRODUCT17_CODE'] = 'PRODUCT17_CODE';
		$expColumns['PRODUCT17_QUANTITY'] = 'PRODUCT17_QUANTITY';
		$expColumns['PRODUCT17_AMOUNT'] = 'PRODUCT17_AMOUNT';
		$expColumns['PRODUCT18_CODE'] = 'PRODUCT18_CODE';
		$expColumns['PRODUCT18_QUANTITY'] = 'PRODUCT18_QUANTITY';
		$expColumns['PRODUCT18_AMOUNT'] = 'PRODUCT18_AMOUNT';
		$expColumns['PRODUCT19_CODE'] = 'PRODUCT19_CODE';
		$expColumns['PRODUCT19_QUANTITY'] = 'PRODUCT19_QUANTITY'; 
		$expColumns['PRODUCT19_AMOUNT'] = 'PRODUCT19_AMOUNT';
		$expColumns['PRODUCT20_CODE'] = 'PRODUCT20_CODE';
		$expColumns['PRODUCT20_QUANTITY'] = 'PRODUCT20_QUANTITY';
		$expColumns['PRODUCT20_AMOUNT'] = 'PRODUCT20_AMOUNT';*/
		$expColumns['VISA_MERCHANT_ID'] = 'VISA_MERCHANT_ID';
		$expColumns['CASH_BONUS'] = 'CASH_BONUS';
		$expColumns['DEFERRED_FACTOR'] = 'DEFERRED_FACTOR';
		$expColumns['DEFERRED_TERM_LENGTH'] = 'DEFERRED_TERM_LENGTH';
		
		$expColumns['TRANSMISSION_TIMESTAMP'] = 'TRANSMISSION_TIMESTAMP';
		$expColumns['TRAN_FEE_AMOUNT'] = 'TRAN_FEE_AMOUNT';
		//$expColumns['CVC_DATA'] = 'CVC_DATA';
		$expColumns['VERIFY_MAG_STRIPE'] = 'VERIFY_MAG_STRIPE';
		$expColumns['SAF'] = 'SAF';
		$expColumns['SECURITY_RESPONSE_CODE'] = 'SECURITY_RESPONSE_CODE';
		
		$expColumns['SOURCE_PIN_FORMAT'] = 'SOURCE_PIN_FORMAT';
		//$expColumns['DEST_PIN_FORMAT'] = 'DEST_PIN_FORMAT';
		$expColumns['VOICE_AUTH'] = 'VOICE_AUTH';
		$expColumns['ACTUAL_AMOUNT'] = 'ACTUAL_AMOUNT';
		$expColumns['REVERSAL_AMOUNT'] = 'REVERSAL_AMOUNT';
		$expColumns['VOICE_AUTH_TEXT'] = 'voice Auth Text';
		$expColumns['OPERATOR_ID'] = 'Operator Id';
		$expColumns['WORKSTATION'] = 'Workstation';
		$expColumns['ORIG_AUTH_NUMBER'] = 'Orig Auth Number';
		$expColumns['TICKET_NBR'] = 'Ticket Nbr';
		$expColumns['SYSTEM_DATE'] = 'System Date';
		$expColumns['DEF_GROSS_AMT'] = 'Def Gross Amt';
		$expColumns['MONTHLY_AMORT_AMT'] = 'Monthly Amort Amt';
		$expColumns['PURGE_DATE'] = 'Purge Date';
		
		return $expColumns;
	}
}

?>