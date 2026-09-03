/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-03-29 15:08:16
Description: 根据材料号查找低倍详细信息，用来生成wrod报表
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts27_mat_detail)
int f_qmts27_mat_detail(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		sqlstr = "SELECT REC_CREATOR,REC_REVISOR,REC_REVISE_TIME,ARCHIVE_FLAG,ARCHIVE_STAMP_NO,COMPANY_CODE,COMPANY_NAME,HEAT_NO,ST_NO,MAT_NO,MAT_THICK,CENTER_SGRG_POROSITY,CRACK_CENTER,EQUIAXED_GRAIN_WIDTH,EQUIAXED_GRAIN_PERCENTAGE,TRI_CRACK_GRADE,ANGLE_CRACK_GRADE,TRANSVERSE_INTERNAL_CRACK,LONGITUDINAL_INTERNAL_CRACK,OTHER_DEFECTS_DESCRIPTION,CRACK,INNER_ARC_WIDTH,OUTTER_ARC_WIDTH,CENTRE_THICKNESS,EDGE_THICKNESS,TEST_PROCEDURE,REFERENCE_STANDARD,CHECK_MAKER,DEV_CODE,C_DIV,AREA,SAMPLE_MAKER,EQUIAXED_GRAIN_PERCENTAGE_R,SUBSTR(REC_CREATE_TIME, 1, 4) || '-' || SUBSTR(REC_CREATE_TIME, 5, 2) || '-' || SUBSTR(REC_CREATE_TIME, 7, 2) AS CREATE_DATE FROM tqmts27 WHERE MAT_NO =@mat_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("mat_no", mat_no);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		if (bcls_ret->Tables[0].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "材料号[%s]没有低倍硫印实绩", (const char*)mat_no);
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
