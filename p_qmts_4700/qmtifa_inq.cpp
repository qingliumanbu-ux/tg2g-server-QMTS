/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 任龙
日期: 2012-05-02
功能: 查询氧化铁粉检化验数据
修改历史：
	日期:________；修改人：________; 需求提出人________
	变更内容:

*************************************************/

/*<remark>=========================================================
/// <summary>
/// 查询氧化铁粉检化验数据
/// <para>
/// 1. 读取前台传入参数；
/// 2. 拼接SQL；
/// 3. 执行查询，返回查询结果。
/// </para>
/// </summary>
/// <param name="selectCols">要查询的列名</param>
/// <returns>查询氧化铁粉检化验数据</returns>
===========================================================</remark>*/
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company: 上海宝信软件股份有限公司
/// Author:  任龙
/// Version:  1.0
/// History:
///	2012-03-16 任龙[创建] 
#include "stdafx.h"


BM2F_ENTERACE(qmtifa_inq);

int f_qmtifa_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);
	int  doFlag = 0;
		CDateTime dt1 = CDateTime::Now();
	try
	{
		int type  = bcls_rec->Tables[0].Rows[0]["type"];	//查询分类（0-多笔；1-单笔）

		if(type == 0)
		{
			//接收从前台传入的值
			//前台拼接sql,查询数据
			CString sql_selectCols=(CString)bcls_rec->Tables[0].Rows[0]["selectCols"].ToString().Trim();
			CString sql_fromTable=(CString)bcls_rec->Tables[0].Rows[0]["tableName"].ToString().Trim();
			CString sql_orderBy=" ORDER BY " + sql_fromTable + ".LOT_NO_PE ";

			CDbCommand cmd(conn);
			CString sql="";
			CString sql_where ="1=1";
			CString sql_count = "";

			/*获取查询条件传入列数*/
			int count_col = bcls_rec->Tables[2].Columns.get_Count();
			
			for(int i = 0;i < count_col;i++)
			{   
				/*如果查询条件的值为空则跳出*/
				if(bcls_rec->Tables[2].Rows[0][i].ToString().Trim().GetLength() == 0)
				{
					continue;
				}
				/*拼接查询的条件*/
				if(bcls_rec->Tables[2].Columns[i].get_ColumnName() == "DATE_FROM")
				{
					sql_where += " AND SUBSTR(REC_REVISE_TIME,1,8) >= @"+bcls_rec->Tables[2].Columns[i].get_ColumnName();
					cmd.Parameters.Set(bcls_rec->Tables[2].Columns[i].get_ColumnName(), bcls_rec->Tables[2].Rows[0][i]);
				}
				else if(bcls_rec->Tables[2].Columns[i].get_ColumnName() == "DATE_TO")
				{
					sql_where += " AND SUBSTR(REC_REVISE_TIME,1,8) <= @"+bcls_rec->Tables[2].Columns[i].get_ColumnName();
					cmd.Parameters.Set(bcls_rec->Tables[2].Columns[i].get_ColumnName(), bcls_rec->Tables[2].Rows[0][i]);
				}
				else
				{
					sql_where += " AND " + bcls_rec->Tables[2].Columns[i].get_ColumnName() + " LIKE @"+bcls_rec->Tables[2].Columns[i].get_ColumnName() + " || '%' ";
					
					cmd.Parameters.Set(bcls_rec->Tables[2].Columns[i].get_ColumnName(), bcls_rec->Tables[2].Rows[0][i]);
					
				}
			}
			//起始-终止之间
			int nStar = (int)bcls_rec->Tables[1].Rows[0]["START"];
			int nPageSize = (int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
			int nfetchRows = nStar + nPageSize + 1;
			CString nfetchRowsStr = CConvert::ToString(nfetchRows);

			if(sql_selectCols == "")
			{
				sql_selectCols="*";
			}
			//拼接SQL
			sql="SELECT "+sql_selectCols+" FROM "+sql_fromTable+" WHERE "+sql_where+" "+sql_orderBy+ " FETCH FIRST "+ nfetchRowsStr +" ROWS ONLY";
			sql_count="SELECT COUNT(1) FROM "+sql_fromTable+" WHERE "+sql_where;

			CDecimal rc=0;
			cmd.SetCommandText(sql_count);
			if(sql_count!="")
			{
				//Log::Trace("QMTI", "f_qmtifa_inq","开始查询[%s]", (const char*)sql_count);
				rc=cmd.ExecuteScalar();
				//把值压入RC中，传出前台
				bcls_ret->ExtendedProperties.Add("RC",rc.ToString());
			}
			else
			{
				bcls_ret->ExtendedProperties.Add("RC","0");
			}
	        

			//zl新增返回总页数
			CDecimal tp = 0;
			if (rc.ToInt32() % nPageSize == 0)
			{
				tp = rc.ToInt32() % nPageSize;
			}
			else
			{
				tp = rc.ToInt32() % nPageSize + 1;
			}
			bcls_ret->ExtendedProperties.Add("TP",tp.ToString());
			

			cmd.SetCommandText(sql);
			Log::Trace("QMTI", "f_qmtifa_inq","开始查询[%s]", (const char*)sql);
			int count=cmd.ExecuteQuery(bcls_ret->Tables[0],nStar,nPageSize);

			Log::Trace("QMTI", "f_qmtifa_inq","共查询了[%d]条数据",count);
			//sprintf(s.msg,"查询成功！");
			strcpy(s.msg,_RES("GCRSS0000002")/*处理成功*/);
			Log::Trace("QMTI", "f_qmtifa_inq","查询完成，耗时[%f]秒",(CDateTime::Now() - dt1).TotalSeconds());
		}

		if(type == 1)
		{
			//接收从前台传入的值
			//前台拼接sql,查询数据
			CString sql_selectCols  =  (CString)bcls_rec->Tables[0].Rows[0]["selectCols"].ToString().Trim();
			CString sql_fromTable   =  (CString)bcls_rec->Tables[0].Rows[0]["tableName"].ToString().Trim();
			CString lot_no_pe       =  bcls_rec->Tables[1].Rows[0]["LOT_NO_PE"].ToString();
			//建立连接
			CDbCommand cmd(conn);
			CString sql="";
			CString sql_data = "";
			CString sql_where ="1=1";

			//生成where语句
			sql_where += " AND LOT_NO_PE = @lot_no_pe";
			cmd.Parameters.Set("lot_no_pe", lot_no_pe);


			if(sql_selectCols == "")
			{
				sql_selectCols="*";
			}
			//拼接SQL
			sql="SELECT " + sql_selectCols + " FROM " + sql_fromTable + " WHERE "+sql_where;

			Log::Trace("QMTI", "f_qmtifa_inq","sql_拼接[%s]",(const char*)sql_where);

			cmd.SetCommandText(sql);
			Log::Trace("QMTI", "f_qmtifa_inq","开始查询[%s]", (const char*)sql);
			int count=cmd.ExecuteQuery(bcls_ret->Tables[0]);
			
			Log::Trace("QMTI", "f_qmtifa_inq","共查询了[%d]条数据",count);
			//sprintf(s.msg,"查询成功！");
			strcpy(s.msg,_RES("GCRSS0000002")/*处理成功*/);
			Log::Trace("QMTI", "f_qmtifa_inq","查询完成，耗时[%f]秒",(CDateTime::Now() - dt1).TotalSeconds());
		}

	}

	//try块程序发生异常
	catch(const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg,"%s:%s",(const char*)ex.GetSource(),(const char*)ex.GetMsg());

	}

	//平台异常
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}
