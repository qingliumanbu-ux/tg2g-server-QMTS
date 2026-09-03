/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-12
Description: 工序成分标准修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 工序成分标准修改
/// <para>
/// 获取输入参数：TQMTS02(成分标准) ；
/// </para>
/// <para>数据库表：TQMTS02(成分标准)); 
///  前台画面QMTS02成分标准F5(修改)调用  </para>
/// </summary>
/// <param name="TQMTS02">成分标准    </param>
===========================================================</remark>*/  

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts02_upd)

int f_qmts02_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int elm_num = 0;
	int insert_flag = 0;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	CString sqlstr = "";
	CString base_code = "";
	
	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			//取得单行传入信息
			tqmts02.Reset();
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i-1]);
			base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
			tqmts02["BASE_CODE"] = base_code;
			Log::Trace("", "","qmts02_upd IN:---ST_NO = [{0}]",(const char*)tqmts02["ST_NO"].ToString());
			Log::Trace("", "","qmts02_upd IN:---FACTORY_DIV = [{0}]",(const char*)tqmts02["FACTORY_DIV"].ToString());
			Log::Trace("", "","qmts02_upd IN:---WHOLE_BACKLOG_CODE = [{0}]",(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString());
			Log::Trace("", "", "qmts02_upd IN:---BASE_CODE = [{0}]", (const char*)base_code);

			if(tqmts02["ST_NO"].ToString().Trim() == "" || tqmts02["ST_NO"].ToString().Trim() == " ")
			{
				sprintf(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if(tqmts02["FACTORY_DIV"].ToString().Trim() == "" || tqmts02["FACTORY_DIV"].ToString().Trim() == " ")
			//{
			//	sprintf(s.msg,_RES("QM00S0004175")/*传入厂别区分不可为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			if(tqmts02["WHOLE_BACKLOG_CODE"].ToString().Trim() == "" || tqmts02["WHOLE_BACKLOG_CODE"].ToString().Trim() == " ")
			{
				sprintf(s.msg,_RES("QM00S0004177")/*工序代码不能为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//调用检查出钢记号是否在计划中使用的函数
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts02["FACTORY_DIV"];
			bcls_rec_s.Tables[1].Rows.Add();
			bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = tqmts02["ST_NO"];
			doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
			if(doFlag != 0)
			{    
				throw CApplicationException(-1, s.msg, log.Location);
			}

			insert_flag = 0;
			Log::Trace("","","1=============");
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TQMTS02 "
						"  WHERE FACTORY_DIV = @factory_div "
						"    AND ST_NO = @st_no "
						"    AND BASE_CODE =@base_code"
						"    AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						"  FETCH FIRST ROW ONLY ";
					break;

				//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				//	sqlstr = " SELECT * "
				//		"   FROM TQMTS02 "
				//		"  WHERE FACTORY_DIV = @factory_div "
				//		"    AND ST_NO = @st_no "
				//		"    AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				//		"  FETCH FIRST ROW ONLY ";
				//	break;

				//case DB_KIND_MSSQL:				// MS SQL Server数据库
				//case DB_KIND_ORACLE:	        // Oracle 数据库

				//default:						// 所有数据库适用，通用SQL语句
				//	sqlstr = " SELECT * "
				//		"   FROM TQMTS02 "
				//		"  WHERE FACTORY_DIV = @factory_div "
				//		"    AND ST_NO = @st_no "
				//		"    AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				//		"    AND ROWNUM = 1 ";
				//	break;
			}
			Log::Trace("", "", "TQMTS02 sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
			cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts02);
			}
			cmd_inq.Close();
			
			//-------------------- 删除 --------------------
			tqmts02.Delete("ST_NO, WHOLE_BACKLOG_CODE, FACTORY_DIV,BASE_CODE"); //条件字段项
			
			//-------------------- 新增 --------------------
			tqmts02["REC_REVISOR"] = s.userid;
			tqmts02["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts02["DU_FLAG"] = " ";
			tqmts02["DU_MAKER"] = " ";
			tqmts02["DU_TIME"] = " ";
			tqmts02["VERSION"] = tqmts02["VERSION"].ToDecimal() + 1;
			tqmts02["BASE_CODE"] = base_code;

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TEP0002 "
							 "  WHERE CODE_CLASS = 'QMYS' "
							 "    AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
							 "  ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while(cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002);
				
				tqmts02["ELM_CODE"] = tep0002["CODE"];
				tqmts02["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
				tqmts02["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
				tqmts02["ELM_UNIT"] = "%";

				elm_num = 0;
				//元素主试最小值
				tqmts02["MAIN_MIN"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_MAIN_MIN"].ToDecimal();
				if(tqmts02["MAIN_MIN"].ToDecimal() != -1)
				{
					elm_num++;
				}
				else
				{
					tqmts02["MAIN_MIN"] = 0.0;
				}

				//元素主试最大值
				tqmts02["MAIN_MAX"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_MAIN_MAX"].ToDecimal();
				if(tqmts02["MAIN_MAX"].ToDecimal() != -1)
				{
					elm_num++;
				}
				else
				{
					tqmts02["MAIN_MAX"] = 99.999;
				}

				//元素主试目标值
				tqmts02["MAIN_AIM"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_MAIN_AIM"].ToDecimal();
				if(tqmts02["MAIN_AIM"].ToDecimal() != -1)
				{
					elm_num++;
				}
				else
				{
					tqmts02["MAIN_AIM"] = 0.0;
				}

				//元素特采最小值
				tqmts02["SPE_MIN"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_SPE_MIN"].ToDecimal();
				if(tqmts02["SPE_MIN"].ToDecimal() != -1)
				{
					elm_num++;
				}
				else
				{
					tqmts02["SPE_MIN"] = 0.0;
				}

				//元素特采最大值
				tqmts02["SPE_MAX"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_SPE_MAX"].ToDecimal();
				if(tqmts02["SPE_MAX"].ToDecimal() != -1)
				{
					elm_num++;
				}
				else
				{
					tqmts02["SPE_MAX"] = 99.999;
				}

				//数据效验
				if(tqmts02["MAIN_MIN"].ToDecimal()  >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_AIM"].ToDecimal() >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_MIN"].ToDecimal() >tqmts02["MAIN_AIM"].ToDecimal())
				{
					CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
					CMessageFormat::Format(s.msg, _RES("QM00S0004178")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素主试成分数据倒置。*/, arguments, 3);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if(tqmts02["SPE_MIN"].ToDecimal()  >tqmts02["SPE_MAX"].ToDecimal() )
				{
					CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
					CMessageFormat::Format(s.msg, _RES("QM00S0004001")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素特采成分数据倒置。*/, arguments, 3);
					throw CApplicationException(-1, s.msg, log.Location);
				}				

				//元素判定及打质保书标记
				tqmts02["SMELT_CHEMI_FLAG"] = bcls_rec->Tables[0].Rows[i-1][tep0002["CODE"].ToString() + "_smelt_chemi_flag"].ToString().Trim();

				Log::Trace("", "", "elm_num[{0}],tqmts02.SMELT_CHEMI_FLAG[{1}]", elm_num, tqmts02["SMELT_CHEMI_FLAG"].ToString());
				//如果某一元素的5项指标都为空，则不加该项数据
				if(elm_num > 0)
				{
					if(tqmts02["SMELT_CHEMI_FLAG"].ToString().TrimOrBlank() == "-1")
					{
						CFormattable arguments[] = { (const char*)tqmts02["ELM_NAME"].ToString() };
						CMessageFormat::Format(s.msg, "元素[{0}]有标准,不能没有判定打质保书标记！", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//写入成分标准表
					tqmts02.TrimOrBlank();
					tqmts02.Insert();
					insert_flag = 1;
				}
			}
			cmd_inq.Close();

			if(insert_flag == 0)
			{
				strcpy(s.msg,"元素标准不能全为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
