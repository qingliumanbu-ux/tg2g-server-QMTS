/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-09
Description: 渣样实绩后备修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///渣样实绩后备修改
/// <para>
/// 1.渣样实绩后备修改
/// 
/// </para>
/// <para>数据库表：TQMTS26(实绩_渣样成分)				</para>
/// <para>主调用函数：前台QMTS26画面的F4(修改)调用		</para>
/// <para>需调用函数：									/para>
/// </summary>
/// <param name="heat_no">熔炼号				</param>
/// <param name="st_sample_no">试样号			</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts26_upd)


int f_qmts26_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int elm_num = 0;//实绩元素数量

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts26("TQMTS26");
	CModel tep0002("TEP0002");
	CModel tqmts24("TQMTS24");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{
			/* 取得单行传入信息 */
			tqmts24.Reset();
			tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts26_upd IN:---heat_no[{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "","qmts26_upd IN:---st_sample_no[{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());
			Log::Trace("", "","qmts26_upd IN:---analyse_time[{0}]",(const char*)tqmts24["ANALYSE_TIME"].ToString());
			Log::Trace("", "","qmts26_upd IN:---sample_ifgood[{0}]",(const char*)tqmts24["SAMPLE_IFGOOD"].ToString());
			
			//校验传入参数
			if(tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts24["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if(tqmts24["ANALYSE_TIME"].ToString().TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg,_RES("QM00S0004335")/*分析时间不允许为空。*/); 
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//if(tqmts24["SAMPLE_IFGOOD"].ToString().TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg,"试样良否不允许为空!"); 
				//throw CApplicationException(-1, s.msg, log.Location);
			//}

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TQMTS24 "
							 " WHERE HEAT_NO = @heat_no "
							 "   AND ST_SAMPLE_NO = @st_sample_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("st_sample_no", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts24);
			}
			else
			{
				sprintf(s.msg, "熔炼号[%s]的渣样实绩不存在，请先新增！",(const char*)tqmts24["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();

			tqmts24["REC_REVISOR"] = s.userid;
			tqmts24["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			if(tqmts24["ANALYSE_TIME"].ToString().TrimOrBlank() == " ")
			{
				tqmts24["ANALYSE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			}

			tqmts24.Delete("HEAT_NO, ST_SAMPLE_NO"); //条件字段项

			tqmts26["REC_CREATOR"]			= tqmts24["REC_CREATOR"];
			tqmts26["REC_CREATE_TIME"]		= tqmts24["REC_CREATE_TIME"];
			tqmts26["REC_REVISOR"]			= tqmts24["REC_REVISOR"];
			tqmts26["REC_REVISE_TIME"]		= tqmts24["REC_REVISE_TIME"];
			tqmts26["ARCHIVE_FLAG"]		= tqmts24["ARCHIVE_FLAG"];
			tqmts26["PONO"]				= tqmts24["PONO"];
			tqmts26["HEAT_NO"]				= tqmts24["HEAT_NO"];
			tqmts26["ST_SAMPLE_NO"]		= tqmts24["ST_SAMPLE_NO"];
			tqmts26["ST_SAMPLE_NAME"]		= " ";
			tqmts26["ST_NO"]				= tqmts24["ST_NO"];
			tqmts26["WHOLE_BACKLOG_CODE"]	= tqmts24["WHOLE_BACKLOG_CODE"];

			tqmts26.Delete("HEAT_NO, ST_SAMPLE_NO"); //条件字段项

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QMZS' "
							 "   AND TRIM(CODE_DESC_2_CONTENT) is not null "
							 " ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002);
				tqmts26["ELM_ACT"] = bcls_rec->Tables[0].Rows[i]["ELM_ACT"].ToString().TrimOrBlank(); //根据元素代码得到前台传入的元素值
				Log::Trace("", "", "ELM_CODE[{0}], ELM_NAME[{1}], ELM_ACT[{2}]", (const char*)tqmts26["ELM_CODE"].ToString(), (const char*)tqmts26["ELM_NAME"].ToString(), tqmts26["ELM_ACT"].ToDecimal().ToDouble());
				tqmts26["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
				tqmts26["ELM_CODE"] = tep0002["CODE"];
				if (tqmts26["ELM_CODE"].ToString() == bcls_rec->Tables[0].Rows[i]["ELM_CODE"].ToString())
				{
					elm_num++;
					tqmts26.TrimOrBlank();
					tqmts26.Insert();
				}
			}
			cmd_inq.Close();
	 
			if(elm_num<1)
			{
				strcpy(s.msg,_RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts24.TrimOrBlank();
			tqmts24.Insert();
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
