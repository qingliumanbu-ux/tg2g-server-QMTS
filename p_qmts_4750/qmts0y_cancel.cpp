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
#include "tqmts0y.h"

/*<remark>=========================================================
/// <summary>
/// 工艺卡审核取消
/// <para>
/// 1.工艺卡审核取消
/// </para>
/// <para>数据库表：TQMTS0Y(工艺卡表)                      </para>
/// <para>主调用函数：前台QMTS0Y画面的F9(审核取消)调用。   </para>
/// </summary>
/// <param name="SG_CODE"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0y_cancel)

int f_qmts0y_cancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CTQMTS0Y tqmts0y(conn);

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		tqmts0y.SG_CODE = bcls_rec->Tables[0].Rows[0]["SG_CODE"].ToString().Trim();
		tqmts0y.HEAT_ELM_IDX = bcls_rec->Tables[0].Rows[0]["HEAT_ELM_IDX"].ToString().Trim();

		Log::Trace("", "","qmts0y_cancel IN:---SG_CODE = [{0}]", (const char*)tqmts0y.SG_CODE);	
		Log::Trace("", "","qmts0y_del IN:---HEAT_ELM_IDX = [{0}]",(const char*)tqmts0y.HEAT_ELM_IDX);


		//校验出钢记号是否已下发——add by 冯晓轶 2012-04-26
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT VALID_FLAG "
				"   FROM TQMTS0Y "						   
				"  WHERE SG_CODE = @tqmts0y.SG_CODE "
				"  AND HEAT_ELM_IDX = @tqmts0y.HEAT_ELM_IDX";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0y.SG_CODE", tqmts0y.SG_CODE);
		cmd_inq.Parameters.Set("tqmts0y.HEAT_ELM_IDX", tqmts0y.HEAT_ELM_IDX);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts0y.VALID_FLAG = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if(tqmts0y.VALID_FLAG == " ")
		{
			strcpy(s.msg,_RES("QM00S0005926")/*该出钢记号未审核，不需做审核取消。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts0y.VALID_FLAG == "2")
		{
			strcpy(s.msg,_RES("QM00S0005921")/*该出钢记号已下发，不可做审核取消。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tqmts0y.VALID_FLAG = " ";
		tqmts0y.CHECK_MAKER = s.userid;
		tqmts0y.CHECK_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts0y.Update("VALID_FLAG,CHECK_TIME,CHECK_MAKER",
			"SG_CODE, HEAT_ELM_IDX");
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
