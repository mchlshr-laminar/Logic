#include "ProofStatement.hpp"

ProofStatement::ProofStatement(const char* input) : data(this), foundation(input)
{}

ProofStatement::ProofStatement(StatementTree* input) : data(this), foundation(*input)
{}

StatementTree* ProofStatement::getData()
{ return &foundation; }

bool ProofStatement::isJustified()
{ return reason.isJustified(); }
