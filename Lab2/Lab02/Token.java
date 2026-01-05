// Token.java
package Lab02;

import java.lang.Enum;

enum Token {
     
     BOOL("bool"),CHAR("char"), ELSE("else"), FALSE("false"), FLOAT("float"), 
     STRING("string"), IF("if"), INT("int"),  TRUE("true"), WHILE("while"), 
     RETURN("return"), VOID("void"), FUN("fun"),  THEN("then"), LET("let"), 
     IN("in"), END("end"), READ("read"), PRINT("print"), DO("do"),  FOR("for"), 
     EOF("<<EOF>>"), 
     LBRACE("{"), RBRACE("}"), LBRACKET("["), RBRACKET("]"),
     LPAREN("("), RPAREN(")"), SEMICOLON(";"), COMMA(","),
     ASSIGN("="), EQUAL("=="),  LT("<"), LTEQ("<="), GT(">"),  
     GTEQ(">="),  NOT("!"),    NOTEQ("!="), PLUS("+"), MINUS("-"), 
     MULTIPLY("*"), DIVIDE("/"), AND("&"), OR("|"), ID(""), 
     NUMBER(""), STRLITERAL(""); 

    private String value; 

    private Token (String v) {
        value = v;
    }

    public String value( ) { return value; }
    public Token setValue(String v) { 
        this.value = v; 
        return this; 
    }

    public static Token idORkeyword (String name) {
        for (Token token : Token.values()) {
           if (token.value().equals(name)) 
	       return token; //enum으로 정의된 토큰이면 키워드
           if (token == Token.EOF) 
               break;
        }
	    return ID.setValue(name); //키워드가 아니면 ID의 이름을 초기화하고 리턴
    } // keyword or ID
}