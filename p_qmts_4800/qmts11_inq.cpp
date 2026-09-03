/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


// service入口
BM2F_ENTERACE(qmts11_inq)

int f_qmts11_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_steel_grade = "";
	CString s_judgment = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tqmts("TQMTS11");


	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			tqmts.Reset();
			tqmts.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
			if (bcls_rec->Tables[0].Columns.Contains("STEEL_GRADE"))
			{
				s_steel_grade = bcls_rec->Tables[0].Rows[0]["STEEL_GRADE"];
			}
			if (bcls_rec->Tables[0].Columns.Contains("CASTING_PRE_JUDGMENT"))
			{
				s_judgment = bcls_rec->Tables[0].Rows[0]["CASTING_PRE_JUDGMENT"];
			}
			Log::Trace("", "", "{0}", s_steel_grade);
			Log::Trace("", "", "{0}", s_judgment);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				"select * from TQMTS11  where 1=1 ";
			sqlstr_count = " SELECT COUNT(1) FROM TQMTS11  where 1=1 ";
			if (s_steel_grade != "")
			{
				sqlstr_temp += " AND STEEL_GRADE LIKE  '%'||@s_steel_grade ||'%' ";
			}
			if (tqmts["CASTING_PRE_JUDGMENT"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND CASTING_PRE_JUDGMENT =@s_judgment";
			}
			
			sqlstr += sqlstr_temp;
			sqlstr_count += sqlstr_temp;
			sqlstr += " ORDER BY STEEL_GRADE ";
			break;
		}
		cmd_inq.Parameters.Set("s_steel_grade", s_steel_grade);
		cmd_inq.Parameters.Set("s_judgment", s_judgment);
		

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