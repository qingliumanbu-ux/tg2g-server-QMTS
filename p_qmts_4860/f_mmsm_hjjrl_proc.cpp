/************************/
/*** 2024-7-23 ********/
/****   wcm **************/
/****合金加入量  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


BM2_FUNCTION_EXPORT

int f_mmsm_hjjrl_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString str = " ";
	CString heat_no = "";
	CString sta_id = "";
	CString sta_code = "";
	CString st_no = "";
	CString	sqlstr1 = "";
	CString lts_st_no = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK_FG");

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			sta_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("STATION_CODE"))
			sta_code = bcls_rec->Tables[0].Rows[0]["STATION_CODE"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();


		//传LTS获取AOD中得钢种
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr1 = "SELECT ST_NO FROM TMMSM27 WHERE HEAT_NO = '" + heat_no + "'";

		}
		cmd_inq1.SetCommandText(sqlstr1);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			lts_st_no = cmd_inq1.GetString(1);
		}
		cmd_inq1.Close();


		//查询加料是否大于1吨，大于则推送宝武聊天
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT * FROM(SELECT  HEAT_NO,'" + sta_code + "' ASGW,SUM(DEVO_WT) / 1000 AS WT FROM TMMSM2A_YL WHERE HEAT_NO = '" + heat_no + "' AND HEAT_NO LIKE 'A%' AND STATION_ID = '" + sta_id + "'"
				" AND MAT_CODE  IN (SELECT MAT_CODE FROM TQMTSHJ WHERE DEV_CODE='" + sta_id + "')"
				" GROUP BY HEAT_NO, DEV_CODE) WHERE WT>1";

		}
		Log::Trace("", __FUNCTION__, "1216sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			str += "<br>" + cmd_inq.GetString(1);
			if (sta_code == "LTS")
			{
				str += " 钢种" + lts_st_no;
			}
			if (sta_code == "LF")
			{
				str += " 钢种" + st_no;
			}
			str += " " + cmd_inq.GetString(2) + "工位";
			str += " 合金加料" + cmd_inq.GetDecimal(3).ToString() + "吨,需降判";


			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "13";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = cmd_inq.GetString(1);
			bcls_rec_s.Tables[0].Rows[0]["REMARK_FG"] = str;

			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		cmd_inq.Close();

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
