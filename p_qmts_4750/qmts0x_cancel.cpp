/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-26
Description: 工艺卡审核取消
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 工艺卡审核取消
/// <para>
/// 1.工艺卡审核取消
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表)                      </para>
/// <para>主调用函数：前台QMTS0X画面的F9(审核取消)调用。   </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0x_cancel)

int f_qmts0x_cancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString base_code = " ";

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel hqmts0x("HQMTS0X");

	CModel tqmts01("TQMTS01");

	CString sqlstr("");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();

		Log::Trace("", "", "qmts0x_cancel IN:---ST_NO = [{0}]tqmts0x.FACTORY_DIV[{1}]", tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString());

		//厂别区分可能会有多个不能写死，审核必须所有厂别一起审核
		//tqmts0x["FACTORY_DIV"] = "A";

		//校验出钢记号是否已下发——add by 冯晓轶 2012-04-26
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS0X "						   
				"  WHERE ST_NO = @tqmts0x.ST_NO "
				"	 and FACTORY_DIV = @tqmts0x.FACTORY_DIV ";
			if (base_code.Trim() != "")
			{
				sqlstr += "    AND base_code = @base_code ";
			}
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0x);
			cmd_inq.Fetch(hqmts0x);
		}
		cmd_inq.Close();
		if(tqmts0x["VALID_FLAG"].ToString() == "0")
		{
			strcpy(s.msg,_RES("QM00S0005926")/*该出钢记号未审核，不需做审核取消。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts0x["VALID_FLAG"].ToString() == "2")
		{
			strcpy(s.msg,_RES("QM00S0005921")/*该出钢记号已下发，不可做审核取消。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tqmts0x["VALID_FLAG"] = "0";
		tqmts0x["CHECK_MAKER"] = "";
		tqmts0x["CHECK_TIME"] = "";
		tqmts0x.Update("VALID_FLAG,CHECK_TIME,CHECK_MAKER",
			"ST_NO,FACTORY_DIV,BASE_CODE");

		//审核后写入履历

		hqmts0x["DU_FLAG"] = "U";
		hqmts0x["DU_MAKER"] = s.userid;
		hqmts0x["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		hqmts0x.TrimOrBlank();
		hqmts0x.Insert();

		tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		tqmts01["ST_NO"] = tqmts0x["ST_NO"];
		tqmts01["BASE_CODE"] = base_code;
		tqmts01["WHOLE_BACKLOG_CODE"] = "";
		if (base_code.Trim() != "")
		{
			tqmts01.Delete("FACTORY_DIV,ST_NO,BASE_CODE,WHOLE_BACKLOG_CODE");
		}
		else
		{
			tqmts01.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE");
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
