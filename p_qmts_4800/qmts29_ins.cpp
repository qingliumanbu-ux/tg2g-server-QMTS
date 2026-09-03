/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-16
Description: 炉次代表成分新增
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


	
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次代表成分新增
/// <para>
/// 1.炉次代表成分新增
/// 
/// </para>
/// <para>数据库表：TQMTS29(实绩_炉次代表成分)		</para>
/// <para>主调用函数：前台QMTS29画面的F3(新增)调用	</para>
/// <para>需调用函数：								</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号					</param>
/// <param name="ST_NO">  出钢记号					</param>
/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);//判定

// service入口
BM2F_ENTERACE(qmts29_ins)


int f_qmts29_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal v_count = 0;
	CString s_judge_code = "";
	CString s_fin_st_no = "";
	CString h5_flag = "";  // 信融端和winform端有区别 2023/8/22 yangfeng
	int elm_num = 0;//实绩元素数量

	CString sqlstr = "";

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts29("TQMTS29");
	CModel tep0002("TEP0002");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("H5_FLAG"))
		{
			h5_flag = bcls_rec->Tables[0].Rows[0]["H5_FLAG"].ToString().Trim();
		}
		Log::Trace("", "", "h5_flag = {0}", h5_flag);
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{
			/* 取得单行传入信息 */
			tqmts29.Reset();
			tqmts29.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","tqmts29_ins IN:---heat_no[{0}]",(const char*)tqmts29["HEAT_NO"].ToString());

			//校验传入参数
			if(tqmts29["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT COUNT(*) "
							 "  FROM TQMTS29 "
							 " WHERE HEAT_NO = @heat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts29["HEAT_NO"].ToString().Trim());
			v_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if(v_count > 0)
			{
				CFormattable arguments[] = {(const char*)tqmts29["HEAT_NO"].ToString()}; // 定义参数列表的数组
				CMessageFormat::Format(s.msg,_RES("QM00S0005927")/*熔炼号[{0}]的代表成分已存在，不能新增。*/, arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts29["REC_CREATOR"] = s.userid;
			tqmts29["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts29["REC_REVISOR"] = " ";
			tqmts29["REC_REVISE_TIME"] = " ";
			tqmts29["ARCHIVE_FLAG"] = " ";
			tqmts29["FACTORY_DIV"] = "A";
			tqmts29["ELM_OK"] = 0;
			tqmts29["ELM_BACKUP"] = 2;

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT PONO,ST_NO,JUDGE_CODE,FIN_ST_NO "
							 "  FROM TQMTS23 "
							 " WHERE HEAT_NO = @heat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts29["HEAT_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts29["PONO"] = cmd_inq.GetString(1);
				tqmts29["ST_NO"] = cmd_inq.GetString(2);
				s_judge_code = cmd_inq.GetString(3);
				s_fin_st_no = cmd_inq.GetString(4);
			}
			else
			{
				sprintf(s.msg, "熔炼号[%s]不存在！",(const char*)tqmts29["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();

			Log::Trace("", "", "s_judge_code[{0}]", s_judge_code);

			if (s_judge_code.TrimOrBlank() == "1")
			{
				sprintf(s.msg,"熔炼号[%s]代表成分已选定，不能新增炉次代表成分！",(const char*)tqmts29["HEAT_NO"].ToString());  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "","ST_NO[{0}]",(const char*)tqmts29["ST_NO"].ToString());

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QMYS' "
							 "   AND TRIM(CODE_DESC_2_CONTENT) is not null "
							 " ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (h5_flag.Trim() != "")
			{
				Log::Trace("", "", "信融端处理");
				while (cmd_inq.Read())
				{	
					cmd_inq.Fetch(tep0002);
				//	tqmts29["ELM_VALUE"] = bcls_rec->Tables[0].Rows[i]["Q" + tep0002["CODE"].ToString()]; //根据元素代码得到前台传入的元素值,前台没写的传""
				
					if (bcls_rec->Tables[0].Rows[i]["H5_" + tep0002["CODE"].ToString()].ToString().Trim() != "")  // 新增时前台元素没有输入值，传到后台为""
					{
						tqmts29["ELM_VALUE"] = bcls_rec->Tables[0].Rows[i]["H5_" + tep0002["CODE"].ToString()].ToDecimal();
						elm_num++;
						tqmts29["ELM_CODE"] = tep0002["CODE"];
						tqmts29["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
						tqmts29["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
						tqmts29["ELM_UNIT"] = "%";
						Log::Trace("", "", "ELM_CODE[{0}], ELM_NAME[{1}], ELM_VALUE[{2}]", (const char*)tqmts29["ELM_CODE"].ToString(), (const char*)tqmts29["ELM_NAME"].ToString(), tqmts29["ELM_VALUE"].ToDecimal().ToDouble());
						tqmts29.TrimOrBlank();
						tqmts29.Insert();
					}
				}
			}
			else 
			{
				Log::Trace("", "", "winform端处理");
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tep0002);		
					tqmts29["ELM_VALUE"] = bcls_rec->Tables[0].Rows[i][tep0002["CODE"].ToString()].ToDecimal(); //根据元素代码得到前台传入的元素值
					
					if (tqmts29["ELM_VALUE"].ToDecimal() != -1 )  
					{
						elm_num++;
						tqmts29["ELM_CODE"] = tep0002["CODE"];
						tqmts29["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
						tqmts29["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
						tqmts29["ELM_UNIT"] = "%";
						Log::Trace("", "", "ELM_CODE[{0}], ELM_NAME[{1}], ELM_VALUE[{2}]", (const char*)tqmts29["ELM_CODE"].ToString(), (const char*)tqmts29["ELM_NAME"].ToString(), tqmts29["ELM_VALUE"].ToDecimal().ToDouble());
						tqmts29.TrimOrBlank();
						tqmts29.Insert();
					}
				}
			}

			cmd_inq.Close();
	 
			if(elm_num<1)
			{
				strcpy(s.msg,_RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS29";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts29["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts29["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts29["PONO"];

			//启动判定
			doFlag = f_qmts_jud(&bcls_rec_f,&bcls_ret_f,conn);
			if(doFlag != 0)
			{    
				Log::Trace("", "","f_qmts_jud() msg = [{0}]",s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		sprintf(s.msg,_RES("QM00S0004387")/*新增完毕。*/);
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
