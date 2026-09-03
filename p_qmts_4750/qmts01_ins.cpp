/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 工艺卡确认中新增工序制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


	
/*<remark>=========================================================
/// <summary>
/// 工艺卡确认中新增工序制造标准
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)); 
///  前台QMTS01画面的F3(新增)调用    </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts01_ins)

int f_qmts01_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	CString st_no = " ";
	CString factory_div = " ";
	CString base_code = "";
	int doFlag = 0;
	int i;
	CDecimal i_count = 0;
	CDecimal v_count = 0;
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
//	CModel tqmts02("TQMTS02");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{ 
		st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
		Log::Trace("", "", "qmts_ins IN: st_no[{0}]factory_div[{1}]base_code[{2}]",  (const char*)st_no, (const char*)factory_div,(const char*)base_code);
		if (st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tqmts0x["ST_NO"] = st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x["FACTORY_DIV"] = factory_div;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");

		tqmts01.Reset();
		tqmts01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT COUNT(1) "
				"  FROM TQMTS01" 
				" WHERE ST_NO = @st_no "
				" AND FACTORY_DIV =@factory_div"
				" AND WHOLE_BACKLOG_CODE ='' "
				" AND BASE_CODE =@base_code";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("factory_div", factory_div);
		cmd_inq.Parameters.Set("base_code", base_code);
		v_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (v_count > 0)
		{
			sprintf(s.msg, _RES("QM00S0003880")/*出钢记号[{0}]已存在，不能再新增。*/, (const char*)st_no);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tqmts01["REC_CREATOR"] = s.userid;
		tqmts01["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts01["REC_REVISOR"] = " ";
		tqmts01["REC_REVISE_TIME"] = " ";
		tqmts01["ARCHIVE_FLAG"] = " ";
		tqmts01["VERSION"] = 1;
		tqmts01["DU_FLAG"] = " ";
		tqmts01["DU_MAKER"] = " ";
		tqmts01["DU_TIME"] = " ";
		tqmts01["IF_PASS"] = "0";
		tqmts01["IF_PLAN"] = "1";
		tqmts01["IF_MESSAGE"] = "0";
		tqmts01["ST_NO"] = st_no;
		tqmts01["FACTORY_DIV"] = factory_div;
		tqmts01["BASE_CODE"] = base_code;
		tqmts01["WHOLE_BACKLOG_CODE"] = "";
		tqmts01.TrimOrBlank();
		tqmts01.Insert();
		Log::Trace("", "", "新增成功");




			//if(tqmts01["WHOLE_BACKLOG_CODE"].ToString().Trim() == "")
			//{
			//	strcpy(s.msg,_RES("QM00S0004177")/*工序代码不能为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//if(tqmts01["ST_NO"].ToString().GetLength() > 8)
			//{
			//	CFormattable arguments[] = {(const char*)tqmts01["ST_NO"].ToString()};
			//	CMessageFormat::Format(s.msg, _RES("QM00S0004179")/*出钢记号[{0}]不可超出8位。*/, arguments, 1);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//if(tqmts01["WHOLE_BACKLOG_SEQ"].ToDecimal() == 0)
			//{
			//	strcpy(s.msg,_RES("QM00S0004180")/*工序顺序不可为空或0。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//tqmts0x["ST_NO"] = tqmts01["ST_NO"];
			//tqmts0x["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
			//tqmts0x.Query("ST_NO,FACTORY_DIV");;

			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT COUNT(1) "
			//				 "  FROM TEP0002 "
			//				 " WHERE CODE_CLASS  = 'PSS1' "
			//				 "   AND CODE = @whole_backlog_code ";
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("whole_backlog_code", tqmts01["WHOLE_BACKLOG_CODE"].ToString());
			//i_count = cmd_inq.ExecuteScalar();
			//cmd_inq.Close();
			//if(i_count == 0)
			//{
			//	CFormattable arguments[] = {(const char*)tqmts01["WHOLE_BACKLOG_CODE"].ToString()};
			//	CMessageFormat::Format(s.msg, _RES("QM00S0004181")/*工序代码不可为[{0}]。*/, arguments, 1);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			////增加校验，不可重复新增相同的工序

			//i_count=0;
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT COUNT(1) "
			//				 "  FROM TQMTS01 "
			//				 " WHERE ST_NO  = @st_no "
			//				 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
			//cmd_inq.Parameters.Set("whole_backlog_code", tqmts01["WHOLE_BACKLOG_CODE"].ToString());
			//i_count = cmd_inq.ExecuteScalar();
			//cmd_inq.Close();

			//if(i_count > 0)
			//{
			//	CFormattable arguments[] = {(const char*)tqmts01["WHOLE_BACKLOG_CODE"].ToString()};
			//	CMessageFormat::Format(s.msg,_RES("QM00S0006101")/*相同工序代码不可重复录入[{0}]*/, arguments, 1);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}


			//tqmts02["WHOLE_BACKLOG_SEQ"] = tqmts01["WHOLE_BACKLOG_SEQ"];
			//tqmts02["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
			//tqmts02["ST_NO"] = tqmts01["ST_NO"];
			//tqmts02["WHOLE_BACKLOG_CODE"] = tqmts01["WHOLE_BACKLOG_CODE"];

			//tqmts02.Update("WHOLE_BACKLOG_SEQ",  //修改字段项
			//				"FACTORY_DIV, ST_NO, WHOLE_BACKLOG_CODE"); //条件字段项
		
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
