<template>
  <div>
    <div v-for="(value, key) in registers_content" :key="key">
      <p>{{ value }}</p>
    </div>
  </div>
</template>

<script>
export default {
  name: "RegistersComponent",
  props: {
    registers_content: Object,
  },
};
</script>

<style>
p {
    text-align: center;
}
</style>
