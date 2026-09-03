/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-09-22
Description: 查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(qmtscb_inqseaf)

int f_qmtscb_inqseaf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	CString sqlstr3 = "";
	CString sqlstr4 = "";
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString v_from = "";//开始时刻

	CModel tqmtscb1d("TQMTSCB11D_DR");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CString v_operate = "";

	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		
		tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (tqmtscb1d["COMPOSE_LIST_NO2"].ToString().Trim() != "")
			{
				sqlstr_temp += " (SELECT * FROM TQMTSCB11D_DR WHERE AREA_CODE='EAF' AND COMPOSE_LIST_NO2 like '%'||@compose_listno||'%') T ";
			}
			sqlstr_count = " SELECT COUNT(1)+1 FROM" + sqlstr_temp;

			sqlstr1 = "SELECT T.AREA_CODE,T.YIELD,T.MAT_CODE_NAME,T.WEIGHT,T.C_VALUE,T.SI_VALUE,T.MN_VALUE,T.P_VALUE,T.S_VALUE,T.CR_VALUE,T.NI_VALUE,T.MO_VALUE,T.CU_VALUE,T.CO_VALUE,T.MAT_CODE,T.MAT_CLASS_DESC,T.UNIT_PRICE,T.COST FROM" + sqlstr_temp;
			sqlstr2 = "UNION ALL SELECT T.AREA_CODE, null AS YIELD, '电炉配料成分' AS MAT_CODE_NAME, round(sum(T.WEIGHT), 2) as WEIGHT, round(sum(T.C_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as C_VALUE, round(sum(T.SI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as SI_VALUE, round(sum(T.MN_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MN_VALUE, round(sum(T.P_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as P_VALUE, round(sum(T.S_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as S_VALUE, round(sum(T.CR_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CR_VALUE, round(sum(T.NI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as NI_VALUE, round(sum(T.MO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MO_VALUE, round(sum(T.CU_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CU_VALUE, round(sum(T.CO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CO_VALUE, '' as MAT_CODE, '' as MAT_CLASS_DESC, null as UNIT_PRICE, null as COST FROM " + sqlstr_temp + "group by T.area_code";
			/*sqlstr3 = "UNION ALL SELECT T.AREA_CODE, null AS YIELD, '电炉配料成分' AS MAT_CODE_NAME, round(sum(T.WEIGHT), 2) as WEIGHT, round(sum(T.C_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as C_VALUE, round(sum(T.SI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as SI_VALUE, round(sum(T.MN_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MN_VALUE, round(sum(T.P_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as P_VALUE, round(sum(T.S_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as S_VALUE, round(sum(T.CR_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CR_VALUE, round(sum(T.NI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as NI_VALUE, round(sum(T.MO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MO_VALUE, round(sum(T.CU_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CU_VALUE, round(sum(T.CO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CO_VALUE, '' as MAT_CODE, '' as MTART, null as UNIT_PRICE, null as COST FROM " + sqlstr_temp + "group by T.area_code";
			sqlstr4 = "UNION ALL SELECT T.AREA_CODE, null AS YIELD, '电炉配料成分' AS MAT_CODE_NAME, round(sum(T.WEIGHT), 2) as WEIGHT, round(sum(T.C_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as C_VALUE, round(sum(T.SI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as SI_VALUE, round(sum(T.MN_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MN_VALUE, round(sum(T.P_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as P_VALUE, round(sum(T.S_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as S_VALUE, round(sum(T.CR_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CR_VALUE, round(sum(T.NI_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as NI_VALUE, round(sum(T.MO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as MO_VALUE, round(sum(T.CU_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CU_VALUE, round(sum(T.CO_VALUE*T.WEIGHT) / sum(T.WEIGHT), 2) as CO_VALUE, '' as MAT_CODE, '' as MTART, null as UNIT_PRICE, null as COST FROM " + sqlstr_temp + "group by T.area_code";
*/


			sqlstr = sqlstr1 + sqlstr2 + " ORDER BY MAT_CODE";
			break;
		}
			cmd_inq.Parameters.Set("compose_listno", tqmtscb1d["COMPOSE_LIST_NO2"].ToString());
			
		
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}