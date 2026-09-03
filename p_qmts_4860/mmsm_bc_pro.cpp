/************************/
/*** 2024-3-19 ********/
/****   wsl **************/
/**** 镍板库405  ***********/
/************************/


//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天

// service入口
BM2F_ENTERACE(mmsm_bc_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_bc_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString str = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK_NICKEL");

	try
	{
		//查询总数
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  COUNT(*) "
				"FROM  TMMSM60      WHERE 1 = 1   AND BUNKER_TYPE = 'NICKEL' AND STOCK_WT >'0' "
				"AND BUNKER_NO NOT IN('E19', 'E20', 'E21', 'E22', 'F19', 'F20', 'F21', 'F22') order by  BACK_N_1, BUNKER_NO ";
		}
		cmd_inq.SetCommandText(sqlstr);
		int count = cmd_inq.ExecuteScalar().ToInt32();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  BUNKER_NO, MAT_CODE, ROUND(STOCK_WT/1000) "
				"FROM  TMMSM60    WHERE 1 = 1   AND BUNKER_TYPE = 'NICKEL' AND "
				"BUNKER_NO	NOT IN('E19', 'E20', 'E21', 'E22', 'F19', 'F20', 'F21', 'F22') "
				"AND STOCK_WT > '0' "
				"order by  BACK_N_1, BUNKER_NO ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		int i = 0;
		bcls_rec_s.Tables[0].Rows.Add();
		while (cmd_inq.Read())
		{
			i++;
			str += "<br>" + cmd_inq.GetString(1);
			str += " " + cmd_inq.GetString(2);
			str += " " + cmd_inq.GetDecimal(3).ToString()+"吨";
			if (i % 10 == 0 || i == count)
			{
				//发送宝武聊天
				bcls_rec_s.Tables[0].Rows[0]["CODE"] = "10";
				bcls_rec_s.Tables[0].Rows[0]["REMARK"] = datetime + "-" + cmd_inq.GetString(1);
				bcls_rec_s.Tables[0].Rows[0]["REMARK_NICKEL"] = str;

				doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				str = "";
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

