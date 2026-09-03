/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:     yiling
Version:     1.0
Date:        2015-07-13
Description: 根据液压相代码计算温度函数
**************************************************/





//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中

#include "math.h"





BM2_FUNCTION_EXPORT
int f_qmts_count_temp(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int			doFlag = 0;
	int			i = 0;
	int			fetchRowCount = 0;

	CString		s_temp_code = "";	//液压相代码
	CString		s_st_no = "";	//出钢记号
	CDecimal	s_elm_value = 0;
	CDecimal c_value = 0;
	CDecimal	si_elm_value = 0;
	CDecimal mn_value = 0;
	CDecimal s_value = 0;
	CDecimal p_valule = 0;
	CDecimal cu_value = 0;
	CDecimal ni_value = 0;
	CDecimal mo_value = 0;
	CDecimal cr_value = 0;
	CDecimal ti_value = 0;
	CDecimal als_value = 0;
	CModel tqmts0x("TQMTS0X");
	CModel tqmts02("TQMTS02");
	CString		sqlstr = "";

	/* 实体类定义 */

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);



	//输出块
	if (!bcls_ret->Tables[0].Columns.Contains("ELM_VALUE"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ELM_VALUE");
	}
	try
	{
		/*获得传入参数*/

		s_temp_code = bcls_rec->Tables[0].Rows[0]["TEMP_CODE"].ToString().Trim();
		s_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();



		Log::Trace("", __FUNCTION__, "f_qmts_count_temp IN:---s_temp_code		[{0}]", s_temp_code);
		Log::Trace("", __FUNCTION__, "f_qmts_count_temp IN:---s_st_no		[{0}]", s_st_no);


		//校验传入参数
		if ("" == s_temp_code)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数s_temp_code不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == s_st_no)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数s_st_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT * "
				"FROM tqmts02 "
				"WHERE ST_NO = @tqmts02.ST_NO "
				;
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts02);
			if (tqmts02["ELM_NAME"].ToString() == "C")
			{
				c_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Si")
			{
				si_elm_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Mn")
			{
				mn_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "P")
			{
				p_valule = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "S")
			{
				s_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Cu")
			{
				cu_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Ni")
			{
				ni_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Mo")
			{
				mo_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Cr")
			{
				cr_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "Ti")
			{
				ti_value = tqmts02["MAIN_AIM"];
			}
			else if (tqmts02["ELM_NAME"].ToString() == "ALs")
			{
				als_value = tqmts02["MAIN_AIM"];
			}
		}
		cmd_inq.Close();
		if (s_temp_code == "1")
		{
			s_elm_value = 1536 - (78 * c_value + 7.6*si_elm_value + 4.9*mn_value + 34 * p_valule + 30 * s_value + 5 * cu_value + 3.1*ni_value + 2 * mo_value + 1.3*cr_value + 18 * ti_value + 3.6*als_value);
		}
		Log::Trace("", "", "s_elm_value[{0}]", s_elm_value.ToDouble());
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["ELM_VALUE"] = s_elm_value;

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		Log::Trace("", "", "1111111111111111");
		//CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Trace("", "", "%s", (const char*)ex.GetMsg());
		sprintf(s.msg, "erorr[%s]", (const char*)ex.GetMsg());
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		Log::Trace("", "", "s.msg = [%s]", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		Log::Trace("", "", "2222222222222222");
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		Log::Trace("", "", "3333333333333333");
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
