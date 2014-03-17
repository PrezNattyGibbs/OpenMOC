#include "FunctionalMaterial.h"

/**
 * @brief Constructor sets the ID and unique ID for the material.
 * @param id the user-defined id for the material
 */
FunctionalMaterial::FunctionalMaterial(short int id) : Material(id){

  _time = NULL;
  _type = FUNCTIONAL;
  _ts = NULL;
  _gamma = NULL;
  
  _sigma_a_func_temp = false;  
  _sigma_a_func_time = false;
  _sigma_s_func_time = false;
  _conserve_sigma_t  = true;
  _sigma_a_ref = NULL;
  _sigma_s_ref = NULL;

  _temperature = new double[6];
  
  for (int i = 0; i < 6; i++)
    _temperature[i] = 300.0;
  
  return;
}


/**
 * @brief Destructor deletes all cross-section data.
 */
FunctionalMaterial::~FunctionalMaterial() {

  if (_sigma_a_ref != NULL)
    delete [] _sigma_a_ref;  

  if (_sigma_s_ref != NULL)
    delete [] _sigma_s_ref;  
}




/**
 * @brief Set the number of energy groups for this material.
 * @param num_groups the number of energy groups.
 */
void FunctionalMaterial::setNumEnergyGroups(const int num_groups, const int num_time_steps) {

  Material::setNumEnergyGroups(num_groups);
  
  log_printf(DEBUG, "Setting material's num energy groups");
  
  if (num_groups < 0)
    log_printf(ERROR, "Unable to set the number of energy groups for "
	       "material %d to %d", _num_groups);
  
  _num_time_steps = num_time_steps;

  _sigma_a_ref = new double[num_groups*num_time_steps];
  _sigma_s_ref = new double[num_groups*num_groups*num_time_steps];

  for (int i = 0; i < num_groups*num_time_steps; i++)
    _sigma_a_ref[i] = 0.0;

  for (int i = 0; i < num_groups*num_groups*num_time_steps; i++)
    _sigma_s_ref[i] = 0.0;

  _gamma = new double[num_groups];
}


/**
 * @brief Set the material's array of absorption cross-sections.
 * @details This method is intended to be called from 
 * @param xs the array of absorption cross-sections
 * @param num_groups the number of energy groups
 */
void FunctionalMaterial::setSigmaA(double* xs, int num_groups) {

    if (_num_groups != num_groups)
	log_printf(ERROR, "Unable to set sigma_a with %d groups for material "
		   "%d which contains %d energy groups", num_groups,
		   _num_groups);
  
    Material::setSigmaA(xs, num_groups);

    for (int i=0; i < _num_groups; i++)
	_sigma_a_ref[i] = xs[i];
}


/**
 * @brief Set the material's array of scattering cross-sections.
 * @details This method is intended to be called from 
 * @param xs the array of scattering cross-sections
 * @param num_groups the number of energy groups
 */
void FunctionalMaterial::setSigmaS(double* xs, int num_groups_squared) {

    if (_num_groups*_num_groups != num_groups_squared)
	log_printf(ERROR, "Unable to set sigma_s with %d groups for material "
		   "%d which contains %d energy groups", float(sqrt(num_groups_squared)), _id,
		   _num_groups);
  
    Material::setSigmaS(xs, num_groups_squared);
    
    for (int i=0; i < _num_groups; i++) {
      for (int j=0; j < _num_groups; j++){
	_sigma_s_ref[j*_num_groups+i] = xs[i*_num_groups+j];
      }   
    }
}


/**
 * @brief Set the material's array of absorption cross-sections.
 * @details This method is intended to be called from
 * @param xs the array of absorption cross-sections
 * @param num_groups the number of energy groups
 */
void FunctionalMaterial::setSigmaATime(int num_time_steps, int num_groups, double* xs) {

  if (_num_groups != num_groups)
    log_printf(ERROR, "Unable to set sigma_a with %d groups for material "
	       "%d which contains %d energy groups", num_groups,
	       _num_groups);

  /* load _sigma_t_ref with all xs */
  for (int i = 0; i < num_time_steps*num_groups; i++)
      _sigma_a_ref[i] = xs[i];
  
  for (int i = 0; i < _num_groups; i++)
      _sigma_a[i] = xs[i];

  if (_dif_coef != NULL && _buckling != NULL){
    for (int i = 0; i < _num_groups; i++)
      _sigma_a[i] += _dif_coef[i] * _buckling[i];
  }
}


/**
 * @brief Set the material's array of scattering cross-sections.
 * @details This method is intended to be called from
 * @param xs the array of scattering cross-sections
 * @param num_groups the number of energy groups
 */
void FunctionalMaterial::setSigmaSTime(int num_time_steps, int num_groups_squared, double* xs) {

  if (_num_groups != int(sqrt(num_groups_squared)))
    log_printf(ERROR, "Unable to set sigma_s with %d groups for material "
	       "%d which contains %d energy groups", int(sqrt(num_groups_squared)),
	       _num_groups);

  int ng = _num_groups;

  /* set the reference scattering xs array */
  for (int i = 0; i < num_time_steps; i++){
    for (int k=0; k < ng; k++) {
      for (int j=0; j < ng; j++){
	_sigma_s_ref[i*ng*ng + j*ng + k] = xs[i*ng*ng + k*ng + j];
      }   
    }
  }

  /* set the callable scattering xs array */
  for (int k=0; k < ng; k++) {
    for (int j=0; j < ng; j++){
      _sigma_s[j*ng + k] = xs[k*ng + j];
    }   
  }
}


FunctionalMaterial* FunctionalMaterial::clone(){

  FunctionalMaterial* to_mat = new FunctionalMaterial(getId());

  /* copy flags */
  to_mat->sigmaAFuncTime(_sigma_a_func_time);
  to_mat->sigmaAFuncTemp(_sigma_a_func_temp);
  to_mat->sigmaSFuncTime(_sigma_s_func_time);
  to_mat->setConserveSigmaT(_conserve_sigma_t);

  /* set num energy groups */
  to_mat->setNumEnergyGroups(_num_groups, _num_time_steps);

  /* copy xs */
  to_mat->setSigmaT(_sigma_t, _num_groups);
  copySigmaS(to_mat);
  copySigmaSRef(to_mat);
  to_mat->setSigmaF(_sigma_f, _num_groups);
  to_mat->setNuSigmaF(_nu_sigma_f, _num_groups);
  to_mat->setChi(_chi, _num_groups);
  
  if (_buckling != NULL)
    to_mat->setBuckling(_buckling, _num_groups);
				
  if (_dif_coef != NULL)
    to_mat->setDifCoef(_dif_coef, _num_groups);

  if (_sigma_a_func_time)
    to_mat->setSigmaATime(_num_time_steps, _num_groups, _sigma_a_ref);
  else
    to_mat->setSigmaA(_sigma_a_ref, _num_groups);

  to_mat->setTemperature(PREVIOUS, getTemperature(PREVIOUS));
  to_mat->setTemperature(PREVIOUS_CONV, getTemperature(PREVIOUS_CONV));
  to_mat->setTemperature(CURRENT, getTemperature(CURRENT));
  to_mat->setTemperature(FORWARD, getTemperature(FORWARD));
  to_mat->setTemperature(FORWARD_PREV, getTemperature(FORWARD_PREV));
  to_mat->setTemperature(SHAPE, getTemperature(SHAPE));

  if (_gamma != NULL)
    to_mat->setGamma(_gamma, _num_groups);
  
  if (_time != NULL)
    to_mat->setTime(_time, _num_time_steps);

  if (_ts != NULL)
    to_mat->setTimeStepper(_ts);

  return to_mat;
}


void FunctionalMaterial::setTime(double* time, int num_time_steps) {
  
  _time = new double[num_time_steps];
  
  for (int i=0; i < num_time_steps; i++)
    _time[i] = time[i];
}


double* FunctionalMaterial::getTime(){
  return _time;
}


void FunctionalMaterial::sigmaAFuncTemp(bool func_temp){
  _sigma_a_func_temp = func_temp;
}


void FunctionalMaterial::sigmaAFuncTime(bool func_time){
  _sigma_a_func_time = func_time;
}

void FunctionalMaterial::sigmaSFuncTime(bool func_time){
  _sigma_s_func_time = func_time;
}


/* sync current cross sections with current time and temperature */
void FunctionalMaterial::sync(materialState state){
  
    double sigma_s_out;
    double sigma_s_group;
    
    /* SIGMA_A */
    for (int g = 0; g < _num_groups; g++){
    
        sigma_s_out = 0.0;
      
	if (_sigma_a_func_time)
	    _sigma_a[g] = interpolateXS(_sigma_a_ref, state, g);
	else
	    _sigma_a[g] = _sigma_a_ref[g];
	
	if (_sigma_a_func_temp)
	    _sigma_a[g] = _sigma_a[g] * (1.0 + _gamma[g] * 
	                  (pow(getTemperature(state),0.5) - pow(300.0, 0.5)));

	if (_conserve_sigma_t){

	  /* adjust self scattering to conserve total xs */
	  for (int G = 0; G < _num_groups; G++){
            if (G != g)
	      sigma_s_out += _sigma_s[G*_num_groups + g];
	  }
      
	  _sigma_s[g*_num_groups + g] = 1.0 / (3.0 * _dif_coef[g]) - _sigma_a[g] - sigma_s_out;    
	  
	  _sigma_a[g] += _dif_coef[g] * _buckling[g];
	  
	  _sigma_t[g] = _sigma_a[g] + sigma_s_out + _sigma_s[g*_num_groups + g];
	}
	else{

	  sigma_s_group = 0.0;

	  /* sync scattering cross section */
	  if (_sigma_s_func_time){
	    for (int G = 0; G < _num_groups; G++){
	      _sigma_s[G*_num_groups + g] = interpolateScatterXS(_sigma_s_ref, state, g, G);
	      sigma_s_group += _sigma_s[G*_num_groups + g];
	    }
	  }
	  else{
	    for (int G = 0; G < _num_groups; G++){
	      sigma_s_group += _sigma_s[G*_num_groups + g];
	    }
	  }

	  /* recompute sigma_t */
	  _sigma_t[g] = _sigma_a[g] + sigma_s_group;
	}
    }
}


void FunctionalMaterial::setGamma(double* gamma, int num_groups){

  for (int g = 0; g < num_groups; g++)
    _gamma[g] = gamma[g];
}


double* FunctionalMaterial::getGamma(){
  return _gamma;
}


double FunctionalMaterial::interpolateXS(double* xs_ref, materialState state, int group){

  double time = _ts->getTime(state);
  double dt, dxs;
  double xs = xs_ref[group];
  
  for (int i = 1; i < _num_time_steps; i++){
    if (time < _time[i] + 1e-8){
      dt = _time[i] - _time[i-1];
      dxs = xs_ref[i*_num_groups + group] - xs_ref[(i-1)*_num_groups + group];
      xs = xs_ref[(i-1)*_num_groups + group] + (time - _time[i-1]) / dt * dxs;
      break;
    }
  }
  
  return xs;
}


double FunctionalMaterial::interpolateScatterXS(double* xs_ref, materialState state, int group_from, int group_to){

  double time = _ts->getTime(state);
  double dt, dxs;
  int ng = _num_groups;
  double xs = xs_ref[group_to*ng + group_from];


  for (int i = 1; i < _num_time_steps; i++){
    if (time < _time[i] + 1e-8){
      dt = _time[i] - _time[i-1];
      dxs = xs_ref[i*ng*ng + group_to*ng + group_from] - xs_ref[(i-1)*ng*ng + group_to*ng + group_from];
      xs = xs_ref[(i-1)*ng*ng + group_to*ng + group_from] + (time - _time[i-1]) / dt * dxs;
      break;
    }
  }
  
  return xs;
}


void FunctionalMaterial::initializeTransientProps(double num_delay_groups, bool cmfd_mesh){

  _num_delay_groups = num_delay_groups;
  _prec_conc = new double[_num_delay_groups*6];
  _prec_freq = new double[_num_delay_groups*6];

}

void FunctionalMaterial::setPrecConc(materialState state, double conc, int group){
  _prec_conc[(int)state * _num_delay_groups + group] = conc;
}

void FunctionalMaterial::setPrecFreq(materialState state, double freq, int group){
  _prec_freq[(int)state *_num_delay_groups + group] = freq;
}

double FunctionalMaterial::getPrecConc(materialState state, int group){
  return _prec_conc[(int)state *_num_delay_groups + group];
}

double FunctionalMaterial::getPrecFreq(materialState state, int group){
  return _prec_freq[(int)state*_num_delay_groups + group];
}

void FunctionalMaterial::copyPrecConc(materialState state_from, materialState state_to){
  
  for (int dg = 0; dg < _num_delay_groups; dg++)
    _prec_conc[(int)state_to *_num_delay_groups + dg] = _prec_conc[(int)state_from *_num_delay_groups + dg];
}

void FunctionalMaterial::copyPrecFreq(materialState state_from, materialState state_to){
 
 for (int dg = 0; dg < _num_delay_groups; dg++)
    _prec_freq[(int)state_to *_num_delay_groups + dg] = _prec_freq[(int)state_from *_num_delay_groups + dg];
}


void FunctionalMaterial::setTimeStepper(TimeStepper* ts){
  _ts = ts;
}


void FunctionalMaterial::setConserveSigmaT(bool conserve_sigma_t){
  _conserve_sigma_t = conserve_sigma_t;
}


void FunctionalMaterial::copySigmaSRef(Material* material){

  double* xs_ref = material->getSigmaSRef();

  for (int i = 0; i < _num_time_steps*_num_groups*_num_groups; i++)
    xs_ref[i] = _sigma_s_ref[i];
}
